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
	allocVectors();
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
		//if(count == 0)
		//{
		//	std::cout <<  "FUCK"
		//	;
		//}
	}


	for (auto i = 0; i < model_.bone_count; ++i)
	{
		bone_key_frames_[i].sort([](vmd::VmdBoneFrame x, vmd::VmdBoneFrame y)
			{
				return x.frame < y.frame;
			});
		bone_key_frames_itr_[i] = bone_key_frames_[i].begin();
		skeleton_matrix[i] = MathUtil::compose(model_bone_arr[i].position);
		inv_position[i] = MathUtil::compose(-model_bone_arr[i].position);
	}


	// morph
	const auto &morph_frames = motion_ptr->face_frames;

	auto model_morph_arr = model_.morphs.get();

	for (auto frame_index = 0; frame_index < morph_key_frames_.size(); ++frame_index)
	{
		int count = 0;
		for (int morph_index = 0; morph_index < model_.morph_count; ++morph_index)
		{
			if (model_morph_arr[morph_index].morph_name == morph_frames[frame_index].face_name)
			{
				morph_key_frames_[morph_index].push_back(morph_frames[frame_index]);
				count++;
			}
		}
		//if(count == 0)
		//{
		//	std::cout <<  "FUCK"
		//	;
		//}
	}

	for (auto i = 0; i < model_.morph_count; ++i)
	{
		morph_key_frames_[i].sort([](vmd::VmdFaceFrame x, vmd::VmdFaceFrame y)
			{
				return x.frame < y.frame;
			});
		morph_key_frames_itr_[i] = morph_key_frames_[i].begin();
	}
	return true;
}


void MotionController::updateMatrix()
{
	updateChildSkeletonMatrix(root_index_);
	for (int i = 0; i < bone_count_; ++i)
	{
		skeleton_matrix[i] = inv_position[i] * skeleton_matrix[i];
	}
}

void MotionController::updateMorphAnimation(const std::vector<MyMeshData::Vertex> &origin_vertices, std::vector<MyMeshData::Vertex> &morph_vertics)
{
	morph_vertics = origin_vertices;
	for (auto i = 0; i < morph_count_; ++i)
	{
		auto &morph = model_.morphs.get()[i];
		if (morph.morph_type == pmx::MorphType::Bone)
			std::cout << "stop here";
		if (morph.morph_type != pmx::MorphType::Vertex)
			continue;
		if (morph_key_frames_itr_[i] != morph_key_frames_[i].end())
		{
			long t0 = morph_key_frames_itr_[i]->frame;
			float val0 = morph_key_frames_itr_[i]->weight;
			if (++morph_key_frames_itr_[i] != morph_key_frames_[i].end())
			{
				long t1 = morph_key_frames_itr_[i]->frame;
				float val1 = morph_key_frames_itr_[i]->weight;
				float s = (float)(ctime - t0) / (float)(t1 - t0); //Linear Interpolation

				morph_weights_[i] = val0 + (val1 - val0) * s;

				if (ctime != t1) --morph_key_frames_itr_[i];
			}
		}

		for (auto j = 0; j < morph.offset_count; ++j)
		{
			auto &vertex_morph = morph.vertex_offsets.get()[j];
			morph_vertics[vertex_morph.vertex_index].Position.x += vertex_morph.position_offset[0] * morph_weights_[i];
			morph_vertics[vertex_morph.vertex_index].Position.y += vertex_morph.position_offset[1] * morph_weights_[i];
			morph_vertics[vertex_morph.vertex_index].Position.z += vertex_morph.position_offset[2] * morph_weights_[i];
		}
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

				s = MathUtil::bezier(T, param[0], param[4], param[8], param[12]);
				//s = 1 - s;
				skeleton_position[i].x = position0.x + (position1.x - position0.x) * s;
				s = MathUtil::bezier(T, param[1], param[5], param[9], param[13]);
				//s = 1 - s;
				skeleton_position[i].y = position0.y + (position1.y - position0.y) * s;
				s = MathUtil::bezier(T, param[2], param[6], param[10], param[14]);
				//s = 1 - s;
				skeleton_position[i].z = position0.z + (position1.z - position0.z) * s;
				//skeleton_position[i] = position0 + (position1 - position0)*T;

				s = MathUtil::bezier(T, param[3], param[7], param[11], param[15]);
				//s = 1 - s;
				skeleton_rotation[i] = MathUtil::Slerp(rotation0, rotation1, s);
				//skeleton_rotation[i] = rotation0;
				if (ctime != t1) --bone_key_frames_itr_[i];
			}
			if (bone.parent_index >= 0)
			{
				skeleton_position[i] += bone.position - model_.bones[bone.parent_index].position;
			}
			skeleton_locals[i] = MathUtil::compose(skeleton_rotation[i]) * MathUtil::compose(skeleton_position[i]);
		}
	}
	updateIK();
}


void MotionController::updateIK()
{
	XMVECTOR effector_position;
	XMVECTOR target_position;

	XMVECTOR local_effector_position;
	XMVECTOR local_target_position;


	for (size_t i = 0; i < model_.bone_count; i++)
	{
		auto const &IK_bone = model_.bones.get()[i];
		if (IK_bone.bone_flag & 0x0020) // IK type
		{
			auto const &IK_bone_mat = (skeleton_matrix[i]);
			auto const &target_bone = model_.bones.get()[IK_bone.target_index];

			for (int loop_count = 0; loop_count < IK_bone.ik_loop; ++loop_count)
			{
				for (int attention_index = 0; attention_index < IK_bone.ik_link_count; ++attention_index)
				{
					auto const &link = IK_bone.ik_links.get()[attention_index];
					//auto &link_bone = model_.bones.get()[link.link_target];

					auto const &effector_mat = (skeleton_matrix[IK_bone.target_index]);
					effector_position = effector_mat.r[3];
					target_position = IK_bone_mat.r[3];

					auto inv_coord = (XMMatrixInverse(nullptr, skeleton_matrix[link.link_target]));

					local_effector_position = XMVector3Transform(effector_position, inv_coord);
					local_target_position = XMVector3Transform(target_position, inv_coord);

					auto local_effector_direction = XMVector3Normalize(local_effector_position);
					auto local_target_direction = XMVector3Normalize(local_target_position);

					float p = dot(local_effector_direction, local_target_direction);
					if (p > 1 - 1e-6)
						continue;
					float angle = min(acos(p), IK_bone.ik_loop_angle_limit);

					auto axis = XMVector3Cross(local_effector_direction, local_target_direction);
					//if (XMVector3LengthSq(axis).m128_f32[0] < 1e-6)
					//	break;
					auto rotation = XMMatrixRotationAxis(axis, 180.0 / M_PI * angle);

					//if (0)
					if (link.angle_lock)
					{
						auto new_local = rotation * skeleton_locals[link.link_target];
						auto new_euler = MathUtil::fromMatrixToEuler(new_local);
						XMFLOAT3 low(link.min_radian);
						XMFLOAT3 high(link.max_radian);


						auto clamped_euler = XMVectorClamp(new_euler, XMLoadFloat3(&low), XMLoadFloat3(&high));
						auto rotation_mat = XMMatrixRotationRollPitchYawFromVector(clamped_euler);

						//XMVECTOR scal;// not used
						//XMVECTOR quat;// not used
						//XMVECTOR tran;
						//XMMatrixDecompose(&scal, &quat, &tran, new_local);
						auto translation = XMMatrixTranslation(skeleton_locals[link.link_target].r[3].m128_f32[0], skeleton_locals[link.link_target].r[3].m128_f32[1], skeleton_locals[link.link_target].r[3].m128_f32[2]);
						skeleton_locals[link.link_target] = rotation_mat * translation;
					}
					else
					{
						skeleton_locals[link.link_target] = rotation * skeleton_locals[link.link_target];
					}
					updateChildSkeletonMatrix(link.link_target);
				}
				if (XMVector3LengthSq(local_effector_position - local_target_position).m128_f32[0] < 1e-6)
				{
					break;
				}
			}
		}
	}
}

void MotionController::allocVectors()
{
	bone_count_ = model_.bone_count;
	bone_key_frames_.resize(bone_count_);
	bone_key_frames_itr_.resize(bone_count_);
	skeleton_position.resize(bone_count_);
	skeleton_rotation.resize(bone_count_);
	inv_position.resize(bone_count_);
	skeleton_matrix.resize(bone_count_);
	skeleton_locals.resize(bone_count_);
	skeleton_tree_.resize(bone_count_);
	morph_count_ = model_.morph_count;
	morph_key_frames_.resize(morph_count_);
	morph_key_frames_itr_.resize(morph_count_);
	morph_weights_.resize(morph_count_);
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

void MotionController::updateChildSkeletonMatrix(int i)
{
	if (skeleton_tree_[i].parent >= 0)
	{
		skeleton_matrix[i] = skeleton_locals[i] * skeleton_matrix[skeleton_tree_[i].parent];
	}
	else
	{
		skeleton_matrix[i] = skeleton_locals[i];
	}
	for (auto child : skeleton_tree_[i].childs)
	{
		updateChildSkeletonMatrix(child);
	}
}
