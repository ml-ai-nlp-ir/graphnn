#ifndef NNGRAPH_H
#define NNGRAPH_H

#include "i_layer.h"
#include "i_param.h"
#include "cppformat/format.h"
#include <vector>

template<MatMode mode, typename Dtype>
class NNGraph
{
public:
    NNGraph()
    {
        layer_dict.clear();
        ordered_layers.clear();
        name_idx_map.clear();
        has_grad.clear();
    }
    
    void ForwardData(std::map<std::string, IMatrix<mode, Dtype>* > input, Phase phase);   
       
    std::map<std::string, Dtype> ForwardLabel(std::map<std::string, IMatrix<mode, Dtype>* > ground_truth);
            
    void BackPropagation();
    
    inline ILayer<mode, Dtype>* InsertLayer(ILayer<mode, Dtype>* layer, std::vector< ILayer<mode, Dtype>* > operands)
    {
        assert(layer_dict.count(layer->name) == 0);
        layer_dict[layer->name] = layer;
        name_idx_map[layer->name] = ordered_layers.size();
        ordered_layers.push_back(std::make_pair(layer->name, operands));
        return layer;
    }
    
    template<template <MatMode, typename> class LayerType, typename... Args>
    inline ILayer<mode, Dtype>* cl(std::vector< ILayer<mode, Dtype>* > operands, Args&&... args)
    {
        return cl<LayerType>(fmt::sprintf("%s-layer-%d", LayerType<mode, Dtype>::str_type(), layer_dict.size()), 
                             operands, 
                             std::forward<Args>(args)...);
    }
    
    template<template <MatMode, typename> class LayerType, typename... Args>
    inline ILayer<mode, Dtype>* cl(std::string layer_name, 
                            std::vector< ILayer<mode, Dtype>* > operands, 
                            Args&&... args)
    {        
        auto* layer = new LayerType<mode, Dtype>(layer_name, std::forward<Args>(args)...);
        return InsertLayer(layer, operands);   
    }
    
    template<template <MatMode, typename> class LayerType, typename... Args>
    inline ILayer<mode, Dtype>* cl(std::vector< ILayer<mode, Dtype>* > operands,
                                   std::vector< IParam<mode, Dtype>* > params, 
                                   Args&&... args)
    {        
        return cl<LayerType>(fmt::sprintf("%s-layer-%d", LayerType<mode, Dtype>::str_type(), layer_dict.size()),
                             operands, 
                             params,                               
                             std::forward<Args>(args)...);
    }
    // workaround for deducting list
    template<template <MatMode, typename> class LayerType, typename... Args>
    inline ILayer<mode, Dtype>* cl(std::string layer_name,
                                   std::vector< ILayer<mode, Dtype>* > operands,
                                   std::vector< IParam<mode, Dtype>* > params, 
                                   Args&&... args)
    {
        auto* layer = new LayerType<mode, Dtype>(layer_name, params, std::forward<Args>(args)...);
        return InsertLayer(layer, operands);
    }                                   
    
    std::map< std::string, unsigned > name_idx_map;
    std::map< std::string, ILayer<mode, Dtype>* > layer_dict;
    std::vector< std::pair<std::string, std::vector< ILayer<mode, Dtype>* > > > ordered_layers;
    std::vector< bool > has_grad;    
};

#endif