#include "MotionController.h"
#include "mathUtil.h"
#include "../MMDFormats/MikuMikuFormats/Pmd.h"

MotionController::~MotionController()
{
}


bool MotionController::LoadModel(std::wstring model_base, std::wstring pmx_name, std::wstring vmd_name)
{
	model_base_ = model_base;
	std::wstring filename = model_base + pmx_name;
	std::ifstream stream = std::ifstream(filename, std::ios_base::binary);
	model_.Read(&stream);
	//std::vector<std::wstring> aa;
	//for (int i = 0; i < model_.bone_count; ++i)
	//{
	//	aa.push_back(model_.bones.get()[i].bone_name);
	//}
	allocVectors(model_.bone_count);
	std::wstring vmd_path = model_base + vmd_name;
	std::ifstream vstream = std::ifstream(vmd_path, std::ios_base::binary);
	motion_ = vmd::VmdMotion::LoadFromStream(&vstream);
	auto motion_ptr = motion_.get();
	stream.close();
	vstream.close();
	buildSkeletonTree();
	int match_count = 0;
	const auto &bone_frames = motion_ptr->bone_frames;
	auto model_bone_arr = model_.bones.get();
	for (auto frame_index = 0; frame_index < bone_frames.size(); ++frame_index)
	{
		int count = 0;
		for (int bone_index = 0; bone_index < model_.bone_count; ++bone_index)
		{
			if (model_bone_arr[bone_index].bone_name == bone_frames[frame_index].name)
			{
				bone_key_frames_[bone_index].push_back(bone_frames[frame_index]);
				count++;
			}
		}
		if(count == 0)
		{
			std::cout <<  "FUCK"
			;
		}
	}
	for (auto i = 0; i < model_.bone_count; ++i)
	{
		bone_key_frames_[i].sort([](vmd::VmdBoneFrame x, vmd::VmdBoneFrame y)
			{
				return x.frame < y.frame;
			});
		bone_key_frames_itr_[i] = bone_key_frames_[i].begin();
	}
	for (auto i = 0; i < model_.bone_count; ++i)
	{
		skeleton_matrix[i] = MathUtil::compose(model_bone_arr[i].position);
		inv_position[i] = MathUtil::compose(-model_bone_arr[i].position);
	}
	return true;
}


void MotionController::updateMatrix()
{
	calcMatrix(root_index_);
	for (int i = 0; i < bone_count_; ++i)
	{
		skeleton_matrix[i] = skeleton_matrix[i] * inv_position[i];
	}
}

void MotionController::updateBoneAnimation()
{
	for (int i = 0; i < model_.bone_count; ++i)
	{
		auto &bone = model_.bones[i];
		if (bone_key_frames_itr_[i] != bone_key_frames_[i].end())
		{
			long t0 = bone_key_frames_itr_[i]->frame;
			auto rotation0 = bone_key_frames_itr_[i]->orientation;
			auto position0 = bone_key_frames_itr_[i]->position;

			if (++bone_key_frames_itr_[i] != bone_key_frames_[i].end())
			{
				long t1 = bone_key_frames_itr_[i]->frame;
				auto rotation1 = bone_key_frames_itr_[i]->orientation;
				auto position1 = bone_key_frames_itr_[i]->position;

				float T = (float)(ctime - t0) / (float)(t1 - t0);
				float s = 0.0f;
				auto *param = bone_key_frames_itr_[i]->interpolation;

				//s = MathUtil::bezier(T, param[0][0][0], param[0][0][1], param[0][0][2], param[0][0][3]);
				//skeleton_position[i].x = position0.x + (position1.x - position0.x)*s;
				//s = MathUtil::bezier(T, param[0][1][0], param[0][1][1], param[0][1][2], param[0][1][3]);
				//skeleton_position[i].y = position0.y + (position1.y - position0.y)*s;
				//s = MathUtil::bezier(T, param[0][2][0], param[0][2][1], param[0][2][2], param[0][2][3]);
				//skeleton_position[i].z = position0.z + (position1.z - position0.z)*s;

				//s = MathUtil::bezier(T, param[0][3][0], param[0][3][1], param[0][3][2], param[0][3][3]);
				skeleton_position[i] = position0 + (position1 - position0)*T;
				skeleton_rotation[i] = MathUtil::Slerp(rotation0, rotation1,T);
				if (ctime != t1) --bone_key_frames_itr_[i];
			}
			if (bone.parent_index >= 0)
			{
				skeleton_position[i] += bone.position - model_.bones[bone.parent_index].position;
			}
			skeleton_locals[i] = MathUtil::compose(skeleton_rotation[i])*MathUtil::compose(skeleton_position[i]);
		}
	}
}

void MotionController::allocVectors(int bone_size)
{
	bone_count_ = bone_size;
	bone_key_frames_.resize(bone_size);
	bone_key_frames_itr_.resize(bone_size);
	skeleton_position.resize(bone_size);
	skeleton_rotation.resize(bone_size);
	inv_position.resize(bone_size);
	skeleton_matrix.resize(bone_size);
	skeleton_locals.resize(bone_size);
	skeleton_tree_.resize(bone_size);
}

void MotionController::buildSkeletonTree()
{
	auto model_bone_arr = model_.bones.get();
	for (int i = 0; i < bone_count_; ++i)
	{
		if (model_bone_arr[i].parent_index >= 0)
		{
			skeleton_tree_[model_bone_arr[i].parent_index].childs.push_back(i);
			skeleton_tree_[i].parent = model_bone_arr[i].parent_index;
		}
		else
		{
			root_index_ = i;
		}
		if (model_bone_arr[i].parent_index >= 0)
		{
			skeleton_locals[i] = MathUtil::compose(model_bone_arr[i].position - model_bone_arr[model_bone_arr[i].parent_index].position);
		}
		else
			skeleton_locals[i] = MathUtil::compose(model_bone_arr[i].position);
	}
}

void MotionController::calcMatrix(int i)
{
	if (skeleton_tree_[i].parent >= 0)
	{
		skeleton_matrix[i] = skeleton_matrix[skeleton_tree_[i].parent] * skeleton_locals[i];
	}
	else
	{
		skeleton_matrix[i] = skeleton_locals[i];
	}
	for (auto child : skeleton_tree_[i].childs)
	{
		calcMatrix(child);
	}
}
