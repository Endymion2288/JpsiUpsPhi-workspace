#!/bin/bash

rm -f /home/storage0/users/xingcheng/storage2/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/merge_rootfile/datalist_all.txt
find /home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiUpsPhi/rootNtuple/ -mindepth 2 -maxdepth 2 -type d > /home/storage0/users/xingcheng/storage2/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/merge_rootfile/datalist_all.txt

# Create the directory for storing merged result files if it doesn't exist
current_date=$(date +%y%m%d)
output_dir="/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiUpsPhi/HTCondor_merged_rootNtuple_${current_date}"
mkdir -p "$output_dir"

# 定义处理单个文件夹的函数
process_folder() {
    local folder="$1"
    echo "处理文件夹: $folder"
    
    # Find the subfolder with the largest number before the "_" character
    local largest_subfolder=$(ls "$folder" | grep -o '^[0-9]*' | sort -nr | head -n 1)
    
    # Construct the full path to the largest subfolder
    local largest_subfolder_path=$(find "$folder" -maxdepth 1 -type d -name "${largest_subfolder}_*" | head -n 1)

    # Print the largest_subfolder_path
    echo "Largest subfolder path: $largest_subfolder_path"

    # 创建临时目录存储中间结果，避免不同进程冲突
    local temp_dir="${folder}/temp_${RANDOM}"
    mkdir -p "$temp_dir"

    # Merge all .root files in the subfolders of the largest subfolder
    find "${largest_subfolder_path}" -mindepth 1 -maxdepth 1 -type d | while read -r subfolder; do
        if [ -d "$subfolder" ]; then
            subfolder_number=$(basename "$subfolder" | grep -o '[0-9]*$')
            hadd -f "${temp_dir}/tmp_${subfolder_number}_merged.root" "$subfolder"/*.root
        fi
    done

    # Merge all temporary merged files into one final merged file
    hadd -f "${output_dir}/final_merged_${folder##*/}.root" "${temp_dir}/tmp_"*_merged.root

    # Remove temporary directory with all temp files
    rm -rf "${temp_dir}"
}

# 设置并行任务数量 (使用可用CPU核心数或自定义数量)
# 获取CPU核心数
NUM_CORES=$(nproc)
# 可选：留出1-2个核心给系统
MAX_PARALLEL=$(((NUM_CORES - 2) / 2))
# 确保至少有1个核心用于处理
[ "$MAX_PARALLEL" -lt 1 ] && MAX_PARALLEL=1

echo "将使用 $MAX_PARALLEL 个核心并行处理"

# 跟踪正在运行的作业数量
running=0

# 并行处理每个文件夹
while IFS= read -r folder; do
    # 如果已达到最大并行数，等待一个作业完成
    if [ $running -ge $MAX_PARALLEL ]; then
        wait -n  # 等待任意一个子进程完成
        running=$((running - 1))
    fi
    
    # 在后台启动处理
    process_folder "$folder" &
    running=$((running + 1))
    
done < /home/storage0/users/xingcheng/storage2/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/merge_rootfile/datalist_all.txt

# 等待所有剩余作业完成
wait

echo "所有合并任务已完成"