/******************************************************************************
 * [Name of the file]
 *      ParticleCand.C
 * [Purpose]
 *      Implement the class ParticleCand.
 * [Author]
 *      Eric Wang
 * [Date]
 *      20241211
 * [Note]
 * 
******************************************************************************/

#ifndef ParticleCand_C
#define ParticleCand_C

#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/includes/ParticleCand.h"
#include <algorithm>
#include <string>

const std::string ParticleCand::PART_NAME[5] = {"Muon", "Track", "Jpsi", "Upsilon", "Phi"};

ParticleCand& ParticleCand::operator=(const ParticleCand& Source){
    // Check if the source is the same as the target.
    if(this == &Source){
        return *this;
    }
    // Copy the source to the target.
    m_TypeList = Source.m_TypeList;
    m_IdxList.clear();
    for(auto& idxList : Source.m_IdxList){
        m_IdxList.push_back(std::make_shared<PartIdxList_t>(*idxList));
    }
    m_Score   = Source.m_Score;
    m_Id      = Source.m_Id;
    return *this;
}

void ParticleCand::AddParticle(const PartType& arg_Type,
                               const PartIdxList_t& arg_IdxList){
    // Check if the particle type is already in the list
    auto it = std::find(m_TypeList.begin(), m_TypeList.end(), arg_Type);
    if(it != m_TypeList.end()){
        // If there is, merge the list and sort.
        auto idx = std::distance(m_TypeList.begin(), it);
        m_IdxList[idx]->insert(m_IdxList[idx]->end(), arg_IdxList.begin(), arg_IdxList.end());
        // Preserve the order.
        std::sort(m_IdxList[idx]->begin(), m_IdxList[idx]->end());
    }
    else{
        // If not, insert the particle type and the list.
        m_TypeList.push_back(arg_Type);
        m_IdxList.push_back(std::make_shared<PartIdxList_t>(arg_IdxList));
        // Sort the list to make set.
        std::sort(m_IdxList.back()->begin(), m_IdxList.back()->end());
    }
}

void ParticleCand::SetScore(const double& arg_Score){
    m_Score = arg_Score;
}

void ParticleCand::SetId(const unsigned int& arg_Id){
    m_Id = arg_Id;
}

double ParticleCand::GetScore() const{
    return m_Score;
}

unsigned int ParticleCand::GetId() const{
    return m_Id;
}



ParticleCand::PartIdxSet_ptr ParticleCand::GetParticleIdx(const PartType& arg_Type) const{
    // Find the particle type in the list.
    auto it = std::find(m_TypeList.begin(), m_TypeList.end(), arg_Type);
    if(it != m_TypeList.end()){
        // If found, return the corresponding index list.
        return m_IdxList[std::distance(m_TypeList.begin(), it)];
    }
    else{
        // If not found, return an empty list.
        return std::make_shared<PartIdxList_t>();
    }
}

unsigned int ParticleCand::GetParticleIdx(const PartType& arg_Type,
                                          const unsigned int& arg_Index) const{
    // Find the particle type in the list.
    auto it = std::find(m_TypeList.begin(), m_TypeList.end(), arg_Type);
    if(it != m_TypeList.end()){
        // If found, find the index in the corresponding index list.
        auto idx = std::distance(m_TypeList.begin(), it);
        auto it2 = std::find(m_IdxList[idx]->begin(), m_IdxList[idx]->end(), arg_Index);
        if(it2 != m_IdxList[idx]->end()){
            // If found, return the index.
            return *it2;
        }
    }
    // If not found, return -1.
    return -1;
}

bool ParticleCand::Overlap(const ParticleCand& arg_Cand) const{
    // Utilize the static function to compare two candidates.
    return Overlap(*this, arg_Cand);
}

bool ParticleCand::Overlap(const ParticleCand& arg_Cand1,
                           const ParticleCand& arg_Cand2 ){
    // The particle types are ordered in m_TypeList.
    // Use a two-pointer method to look for the same particle type.
    auto it1 = arg_Cand1.m_TypeList.begin();
    auto it2 = arg_Cand2.m_TypeList.begin();
    while(it1 != arg_Cand1.m_TypeList.end() && it2 != arg_Cand2.m_TypeList.end()){
        if(*it1 == *it2){
            // If the particle type is the same, compare the indices.
            if(Overlap(arg_Cand1.m_IdxList[std::distance(arg_Cand1.m_TypeList.begin(), it1)],
                       arg_Cand2.m_IdxList[std::distance(arg_Cand2.m_TypeList.begin(), it2)])){
                return true;
            }
            // Move to the next particle type.
            ++it1;
            ++it2;
        }
        else if(*it1 < *it2){
            // If the particle type in arg_Cand1 is smaller, move to the next one.
            ++it1;
        }
        else{
            // If the particle type in arg_Cand2 is smaller, move to the next one.
            ++it2;
        }
    }
    // If no same particle type is found, return false.
    return false;
}

bool ParticleCand::Overlap(const PartIdxSet_ptr& arg_Set1,
                           const PartIdxSet_ptr& arg_Set2 ){
    // Use a two-pointer method to compare two sets.
    auto it1 = arg_Set1->begin();
    auto it2 = arg_Set2->begin();
    while(it1 != arg_Set1->end() && it2 != arg_Set2->end()){
        if(*it1 == *it2){
            // If the indices are the same, return true.
            return true;
        }
        else if(*it1 < *it2){
            // If the index in arg_Set1 is smaller, move to the next one.
            ++it1;
        }
        else{
            // If the index in arg_Set2 is smaller, move to the next one.
            ++it2;
        }
    }
    // If no same index is found, return false.
    return false;
}

std::string ParticleCand::ToString(){
    // Initialize the output string.
    std::string output;
    // Loop over all particles.
    for(size_t i=0; i < m_TypeList.size(); i++){
        // Print out the particle type.
        output += PART_NAME[static_cast<int>(m_TypeList[i])];
        output += " [ ";
        // Print out the indices.
        for(auto& idx : *m_IdxList[i]){
            output += std::to_string(idx);
            output += " ";
        }
        output += " ] ";
    }
    return output;
}

void ParticleCand::Clear(){
    // Clear the lists.
    m_TypeList.clear();
    m_IdxList.clear();
}


#endif
