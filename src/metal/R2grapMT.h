//
// Created by ingslh on 2023/1/1.
//
#pragma once
#include <string>
#include <vector>

namespace  R2grap{

class R2grapMT {

public:
		R2grapMT(const std::string& filename);

private:
		unsigned window_width_;
		unsigned window_height_;
		unsigned frame_count_;
};
}


