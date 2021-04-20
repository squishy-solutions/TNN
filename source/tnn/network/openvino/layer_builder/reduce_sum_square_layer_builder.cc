// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <cmath>
#include <memory>

#include <ngraph/node.hpp>
#include <ngraph/ngraph.hpp>
#include <ngraph/op/op.hpp>
#include <ngraph/opsets/opset.hpp>
#include <ngraph/opsets/opset1.hpp>
#include <inference_engine.hpp>

#include "tnn/layer/base_layer.h"
#include "tnn/network/openvino/layer_builder/openvino_layer_builder.h"
#include "tnn/extern_wrapper/foreign_blob.h"
#include "tnn/extern_wrapper/foreign_tensor.h"
#include "tnn/network/openvino/openvino_types.h"

namespace TNN_NS {

DECLARE_OPENVINO_LAYER_BUILDER(ReduceSumSquare, LAYER_REDUCE_SUM_SQUARE);

Status ReduceSumSquareOVLayerBuilder::Build() {

    auto paramlist = dynamic_cast<ReduceLayerParam*>(param_);

    if (GetInputNodes().size() <=0) {
        LOGE("Error: 0 input nodes\n");
        return TNNERR_INIT_LAYER;
    }
    auto input_node = GetInputNodes()[0];

    auto squareConst = std::make_shared<ngraph::op::Constant>(
        ngraph::element::Type_t::f32, ngraph::Shape(input_node->get_output_shape(0).size(), 1), std::vector<float>{2.0f});
    auto squareNode = std::make_shared<ngraph::op::v1::Power>(input_node->output(0), squareConst);
    squareNode->validate_and_infer_types();

    auto axisNode = std::make_shared<ngraph::op::Constant>(
        ngraph::element::Type_t::i32, ngraph::Shape{paramlist->axis.size()}, paramlist->axis);
    auto reduceSumNode = std::make_shared<ngraph::op::v1::ReduceSum>(
        squareNode->output(0), axisNode, paramlist->keep_dims);
    reduceSumNode->validate_and_infer_types();

    reduceSumNode->set_friendly_name(paramlist->name);

    ngraph::NodeVector outputNodes;
    outputNodes.push_back(reduceSumNode);
    SetOutputTensors(outputNodes);

    return TNN_OK;
}

REGISTER_OPENVINO_LAYER_BUILDER(ReduceSumSquare, LAYER_REDUCE_SUM_SQUARE);

}