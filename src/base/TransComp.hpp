#pragma once
#include <map>

namespace R2grap{
namespace TransComp{

//adjust two maps have same keys
template<typename Tk, typename Tv>
bool adjustMaps(std::map<Tk,Tv>& map1, std::map<Tk,Tv>& map2){
  if(map1.begin()->first < map2.begin()->first){
    auto it = map1.begin()->first;
    auto map2_k = map2.begin()->first;
    auto map2_v = map2.begin()->second;
    while(it < map2_k){
      map2[it] = map2_v;
      it++;
    }
  }else if(map2.begin()->first < map1.begin()->first){
    auto it = map2.begin()->first;
    auto map1_k = map1.begin()->first;
    auto map1_v = map1.begin()->second;
    while(it < map1_k){
      map1[it] = map1_v;
      it++;
    }
  }

  if(map1.rbegin()->first > map2.rbegin()->first){
    auto it = map1.rbegin()->first;
    auto map2_k = map2.rbegin()->first;
    auto map2_v = map2.rbegin()->second;
    while(it > map2_k){
      map2[it] = map2_v;
      it--;
    }
  }else if(map2.rbegin()->first > map1.rbegin()->first){
    auto it = map2.rbegin()->first;
    auto map1_k = map1.rbegin()->first;
    auto map1_v = map1.rbegin()->second;
    while(it > map1_k){
      map1[it] = map1_v;
      it--;
    }
  }
  return map1.size() == map2.size();
}

template<typename Tk, typename Tv>
bool MapaddMap(std::map<Tk,Tv>& map1, std::map<Tk,Tv>& map2){
  if(map1.size() != map2.size()) return false;
  for(auto& el : map1){
    if(!map2.count(el.first)) return false;
    el.second = el.second + map2[el.first];
  }
  return true;
}

template<typename Tk, typename Tv>
bool MapmultiplyMap(std::map<Tk,Tv>& map1, std::map<Tk,Tv>& map2){
  if(map1.size() != map2.size()) return false;
  for(auto& el : map1){
    if(!map2.count(el.first)) return false;
    el.second = el.second * map2[el.first];
  }
  return true;
}

template<typename Tk, typename Tv>
bool MapdivideNum(std::map<Tk,Tv>& map1, Tv num){
  if(typeid(Tv) == typeid(float) && num == 0) return false;
  for(auto& el : map1){
    el.second = el.second / num;
  }
  return true;
}



}
}