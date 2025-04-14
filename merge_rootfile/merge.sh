#!/bin/bash

rm -f /home/storage0/users/xingcheng/storage2/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/merge_rootfile/datalist_all.txt
find /home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiUpsPhi/rootNtuple/ -mindepth 2 -maxdepth 2 -type d > /home/storage0/users/xingcheng/storage2/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/merge_rootfile/datalist_all.txt

# Create the directory for storing merged result files if it doesn't exist
current_date=$(date +%y%m%d)
output_dir="/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiUpsPhi/HTCondor_merged_rootNtuple_${current_date}"
mkdir -p "$output_dir"

# Read each line from datalist.txt
while IFS= read -r folder; do
    # Find the subfolder with the largest number before the "_" character
    largest_subfolder=$(ls "$folder" | grep -o '^[0-9]*' | sort -nr | head -n 1)
    
    # Construct the full path to the largest subfolder
    largest_subfolder_path=$(find "$folder" -maxdepth 1 -type d -name "${largest_subfolder}_*" | head -n 1)

    # Print the largest_subfolder_path
    echo "Largest subfolder path: $largest_subfolder_path"

    # # Print the subfolders in the largest_subfolder_path
    # echo "Subfolders in $largest_subfolder_path:"
    # find "${largest_subfolder_path}" -mindepth 1 -maxdepth 1 -type d

    # Merge all .root files in the subfolders of the largest subfolder


    find "${largest_subfolder_path}" -mindepth 1 -maxdepth 1 -type d | while read -r subfolder; do
        if [ -d "$subfolder" ]; then
            subfolder_number=$(basename "$subfolder" | grep -o '[0-9]*$')
            hadd -f "${folder}/tmp_${subfolder_number}_merged.root" "$subfolder"/*.root
        fi
    done

    # Merge all temporary merged files into one final merged file
    hadd -f "${output_dir}/final_merged_${folder##*/}.root" "${folder}/tmp_"*_merged.root

    # Remove temporary merged files
    rm "${folder}/tmp_"*_merged.root

done < /home/storage0/users/xingcheng/storage2/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/merge_rootfile/datalist_all.txt