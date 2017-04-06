#include "MotionController.h"
#include "mathUtil.h"
#include "../MMDFormats/MikuMikuFormats/Pmd.h"
#include "UserController.h"

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
	//dumpSkeletonTree("visual.txt");
	int match_count = 0;
	const auto &bone_frames = motion_ptr->bone_frames;
	auto model_bone_arr = model_.bones.get();
	//std::vector<std::wstring> fuck;
	for (auto frame_index = 0; frame_index < bone_frames.size(); ++frame_index)
	{
		int count = 0;
		for (auto bone_index = 0; bone_index < model_.bone_count; ++bone_index)
		{
			if (model_bone_arr[bone_index].bone_name == bone_frames[frame_index].name)
			{
				bone_key_frames_[bone_index].push_back(bone_frames[frame_index]);
				count++;
			}
		}
		//if(count == 0)
		//{
		//	fuck.push_back(bone_frames[frame_index].name);
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
	resetLocals();
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
	updateChildSkeletonMatrix(root_index_);
	if (UserController::getInstance()->useFABRIK)
		updateIK_FABRIK();
	else
		updateIK();
	updateInherited();
}

void MotionController::updateIK()
{
	XMVECTOR effector_position;
	XMVECTOR target_position;

	XMVECTOR local_effector_position;
	XMVECTOR local_target_position;


	for (auto i = 0; i < model_.bone_count; i++)
	{
		auto const &IK_bone = model_.bones.get()[i];
		if (IK_bone.bone_flag & 0x0020) // IK type
		{
			auto &IK_bone_mat = (skeleton_matrix[i]);
			auto const &target_bone = model_.bones.get()[IK_bone.ik_target_bone_index];
			for (int loop_count = 0; loop_count < IK_bone.ik_loop; ++loop_count)
			{
				//for (int attention_index = 0; attention_index < IK_bone.ik_link_count; ++attention_index)
				for (int attention_index = IK_bone.ik_link_count - 1; attention_index >= 0; --attention_index)
				{
					auto const &link = IK_bone.ik_links.get()[attention_index];
					auto &link_bone = model_.bones.get()[link.link_target];

					auto const &effector_mat = (skeleton_matrix[IK_bone.ik_target_bone_index]);
					effector_position = effector_mat.r[3];
					target_position = IK_bone_mat.r[3];
					target_position.m128_f32[0] += UserController::getInstance()->x;
					target_position.m128_f32[1] += UserController::getInstance()->y;
					target_position.m128_f32[2] += UserController::getInstance()->z;

					auto inv_coord = (XMMatrixInverse(nullptr, skeleton_matrix[link.link_target]));

					local_effector_position = XMVector4Transform(effector_position, inv_coord);
					local_target_position = XMVector4Transform(target_position, inv_coord);

					auto local_effector_direction = XMVector3Normalize(XMVector4Normalize(local_effector_position));
					auto local_target_direction = XMVector3Normalize(XMVector4Normalize(local_target_position));

					float p = XMVector3Dot(local_effector_direction, local_target_direction).m128_f32[0];
					if (p > 1 - 1e-6 || p < 1e-6 - 1)
						continue;
					float angle = acos(p);
					//angle = min(acos(p),acos(p));
					angle = min(acos(p), IK_bone.ik_loop_angle_limit);

					auto axis = -XMVector3Cross(local_target_direction, local_effector_direction);
					if (XMVector3LengthSq(axis).m128_f32[0] < 1e-4)
						continue;
					auto rotation = XMMatrixRotationAxis(axis, (float)angle);

					//if (0)
					if (link.angle_lock)
					{
						auto new_local = rotation * skeleton_locals[link.link_target];
						auto new_euler = MathUtil::fromMatrixToEuler2(new_local);
						//auto new_euler = MathUtil::fromMatrixToEuler2(new_local);
						//if(loop_count < 2)
						//{
						//	new_euler.m128_f32[0] -= 1;
						//}

						XMFLOAT3 low(link.min_radian);
						XMFLOAT3 high(link.max_radian);
						if (low.x < -M_PI / 2)
						{
							//auto clamped_euler = _Clamp(new_euler, XMLoadFloat3(&low), XMLoadFloat3(&high));

							//clamped_euler = new_euler;
							skeleton_locals[link.link_target] = rotation * skeleton_locals[link.link_target];
							//auto rotation_mat = XMMatrixRotationX(clamped_euler.m128_f32[0]) * XMMatrixRotationY(clamped_euler.m128_f32[1]) * XMMatrixRotationZ(clamped_euler.m128_f32[2]);
							//auto translation = XMMatrixTranslation(skeleton_locals[link.link_target].r[3].m128_f32[0], skeleton_locals[link.link_target].r[3].m128_f32[1], skeleton_locals[link.link_target].r[3].m128_f32[2]);
							//skeleton_locals[link.link_target] = rotation_mat * translation;
						}
						else
						{
							auto clamped_euler = XMVectorClamp(new_euler, XMLoadFloat3(&low), XMLoadFloat3(&high));

							//clamped_euler = new_euler;

							//auto rotation_mat = XMMatrixRotationX(clamped_euler.m128_f32[0]) * XMMatrixRotationY(clamped_euler.m128_f32[1]) * XMMatrixRotationZ(clamped_euler.m128_f32[2]);
							auto rotation_mat = XMMatrixRotationRollPitchYawFromVector(clamped_euler);
							auto translation = XMMatrixTranslation(skeleton_locals[link.link_target].r[3].m128_f32[0], skeleton_locals[link.link_target].r[3].m128_f32[1], skeleton_locals[link.link_target].r[3].m128_f32[2]);
							skeleton_locals[link.link_target] = rotation_mat * translation;
						}
						//low.x -= 0.5;
						//low.y -= 0.5;
						//low.z = 0;
						//high.x += 0.5;
						//high.y += 0.5;
						//high.z = 0;
					}
					else
					{
						skeleton_locals[link.link_target] = rotation * skeleton_locals[link.link_target];
					}
					updateChildSkeletonMatrix(root_index_);
					//{
					//	auto const &effector_mat = (skeleton_matrix[IK_bone.ik_target_bone_index]);
					//	effector_position = effector_mat.r[3];
					//	target_position = IK_bone_mat.r[3];

					//	auto inv_coord = (XMMatrixInverse(nullptr, skeleton_matrix[link.link_target]));

					//	local_effector_position = XMVector4Transform(effector_position, inv_coord);
					//	local_target_position = XMVector4Transform(target_position, inv_coord);
					//}
				}
				if (XMVector3LengthSq(IK_bone_mat.r[3] - skeleton_matrix[IK_bone.ik_target_bone_index].r[3]).m128_f32[0] < 1e-6)
				{
					break;
				}
			}
		}
	}
}


void MotionController::updateIK_FABRIK()
{
	for (auto i = 0; i < model_.bone_count; i++)
	{
		auto const &IK_bone = model_.bones.get()[i];
		if (IK_bone.bone_flag & 0x0020) // IK type
		{
			auto ik_effector = IK_bone.ik_target_bone_index;
			std::vector<int> bone_nodes;
			bone_nodes.push_back(ik_effector);
			// 极向矢量
			vec3 pole_vector = vec3(0, 0, 1);
			// 极向关节索引
			int pole_effctor_index = -1;
			for (int attention_index = 0; attention_index < IK_bone.ik_link_count; ++attention_index)
			{
				auto const &link = IK_bone.ik_links.get()[attention_index];
				auto &link_bone = model_.bones.get()[link.link_target];
				if (link.angle_lock != 0)
				{
					pole_effctor_index = bone_nodes.size();
					const auto& xmmat = skeleton_matrix[link.link_target];
					pole_vector = (vec4(link.min_radian[2], link.min_radian[1], link.min_radian[0],0) * mat4(vec4(xmmat.r[0]), vec4(xmmat.r[1]), vec4(xmmat.r[2]), vec4(xmmat.r[2]))).xyz().normalized();
				}
				bone_nodes.push_back(link.link_target);
			}
			int ik_root = bone_nodes[IK_bone.ik_link_count];
			float _precision = 10e-6f;
			// IK 目标位置
			auto target_position = GetPositionFromBoneIndex(i);
			// IK 链根节点位置
			auto root_position = GetPositionFromBoneIndex(ik_root);
			// 从根到 IK 目标
			auto root_to_target = target_position - root_position;

			auto root_to_effector = GetPositionFromBoneIndex(ik_effector) - root_position;

			// Maximum length of skeleton segment at full extension
			float maximum_reach = 0;

			int32_t const bone_number = bone_nodes.size();

			// Build the chain
			std::vector<FABRIKChainLink> chain;
			chain.reserve(bone_number);

			for (size_t i = 0; i < bone_number - 1; i++)
			{
				const auto bone = bone_nodes[i];

				auto chain_node = FABRIKChainLink(GetPositionFromBoneIndex(bone),vec3(skeleton_locals[bone].r[3].m128_f32).length(),bone);
				if (chain_node.Length > 0)
				{
					chain.push_back(chain_node);
					maximum_reach += chain_node.Length;
				}
			}

			// 处理 chain root
			{
				const auto bone = bone_nodes[bone_number - 1];
				auto chain_node = FABRIKChainLink(GetPositionFromBoneIndex(bone), vec3(skeleton_locals[bone].r[3].m128_f32).length(), bone);
				chain_node.Length = 0;
				chain.push_back(chain_node);
			}


			bool bone_location_updated = false;
			int32_t const num_chain_links = chain.size();

			float const root_to_target_dist_sq = root_to_target.lengthSquared();
			float maxreach_sq = (maximum_reach * maximum_reach);

			if (root_to_target_dist_sq > maxreach_sq)
			{
				// 目标不可达，全部关节伸直
				for (int32_t LinkIndex = num_chain_links - 2; LinkIndex >= 0; LinkIndex--)
				{
					FABRIKChainLink const &ParentLink = chain[LinkIndex + 1];
					FABRIKChainLink &CurrentLink = chain[LinkIndex];
					CurrentLink.Position = ParentLink.Position + (target_position - ParentLink.Position).normalized() * CurrentLink.Length;
				}
				bone_location_updated = true;
			}
			else
			{
				int32_t const tip_bone_link_index = 0;
				float slop = (chain[tip_bone_link_index].Position - target_position).length();
//				bool singularity = (root_to_target.normalized().dot((chain[num_chain_links - 2].Position - root_position).normalized())) > 0.999f;
//				vec3 singularity_offset;
//				if (singularity)
//				{
//					singularity_offset = -pole_vector * chain[0].Length / 2;
//				}
				if (slop > _precision)
				{
					chain[tip_bone_link_index].Position = target_position;
					int iteration_count = 0;
					float prev_slop = 0;
					while (slop > _precision && abs(prev_slop - slop) > _precision && iteration_count++ <= IK_bone.ik_loop)
					{
//						if (singularity && iteration_count == 1)
//						{
//							chain[tip_bone_link_index].Position += singularity_offset;
//						}
						// "Forward Reaching" stage - adjust bones from end effector.
						for (int LinkIndex = tip_bone_link_index + 1; LinkIndex < num_chain_links - 1; LinkIndex++)
						{
							FABRIKChainLink &CurrentLink = chain[LinkIndex];
							FABRIKChainLink const &ChildLink = chain[LinkIndex - 1];

							auto dir = (CurrentLink.Position - ChildLink.Position).normalized();
							CurrentLink.Position = ChildLink.Position + dir * ChildLink.Length;
						}

						// "Backward Reaching" stage - adjust bones from root.
						for (int LinkIndex = num_chain_links - 2; LinkIndex > tip_bone_link_index; LinkIndex--)
						{
							FABRIKChainLink &CurrentLink = chain[LinkIndex];
							FABRIKChainLink const &ParentLink = chain[LinkIndex + 1];

							CurrentLink.Position = ParentLink.Position + (CurrentLink.Position - ParentLink.Position).normalized() * CurrentLink.Length;
						}

						// Re-check distance between tip location and effector location
						// Since we're keeping tip on top of effector location, check with its parent bone.
						prev_slop = slop;
						slop = abs(chain[tip_bone_link_index].Length - (chain[tip_bone_link_index + 1].Position - target_position).length());
					}

					// Place tip bone based on how close we got to target.
					{
						FABRIKChainLink const &ParentLink = chain[tip_bone_link_index + 1];
						FABRIKChainLink &CurrentLink = chain[tip_bone_link_index];

						CurrentLink.Position = ParentLink.Position + (CurrentLink.Position - ParentLink.Position).normalized() * CurrentLink.Length;
					}

					// 旋转关节 position 以符合极向
					if (pole_effctor_index > 0)
					{
						auto root_to_pole_effector = (chain[pole_effctor_index].Position - root_position).normalized();
						auto dir_effector = cross(root_to_pole_effector, root_to_target).normalized();
						auto dir_target = cross(pole_vector, root_to_target).normalized();

						vec3 const RotationAxis = root_to_target;
						float const RotationAngle = rotate_vectors(RotationAxis, dir_effector, dir_target);

						for (int i = 0; i < chain.size(); ++i)
						{
							auto world_pos = chain[i].Position;
							auto local_pos = world_pos - root_position;
							auto rotate_mat = mat3::fromAxisAngle(RotationAxis, RotationAngle).transposed();
							local_pos = local_pos * rotate_mat;
							chain[i].Position = local_pos + root_position;
						}
					}

					bone_location_updated = true;
				}
			}
			if (bone_location_updated)
			{
				//FABRIK algorithm - re-orientation of bone local axes after translation calculation
				for (int LinkIndex = num_chain_links - 1; LinkIndex > 0; LinkIndex--)
				{
					FABRIKChainLink const &CurrentLink = chain[LinkIndex];
					FABRIKChainLink const &ChildLink = chain[LinkIndex - 1];
//					skeleton_matrix[CurrentLink.bone_index].r[3].m128_f32[0] = CurrentLink.Position.x;
//					skeleton_matrix[CurrentLink.bone_index].r[3].m128_f32[1] = CurrentLink.Position.y;
//					skeleton_matrix[CurrentLink.bone_index].r[3].m128_f32[2] = CurrentLink.Position.z;

					const auto child_position = GetPositionFromBoneIndex(ChildLink.bone_index);
					const auto current_position = CurrentLink.Position;
					// Calculate pre-translation vector between this bone and child
					vec3 const OldDir = (child_position - current_position).normalized();

					// Get vector from the post-translation bone to it's child
					vec3 const NewDir = (ChildLink.Position - CurrentLink.Position).normalized();

					// Calculate axis of rotation from pre-translation vector to post-translation vector
					vec3 const RotationAxis = cross(OldDir, NewDir).normalized();
					float const RotationAngle = acos(dot(OldDir, NewDir));

					XMVECTOR m4;
					m4.m128_f32[0] = RotationAxis.x;
					m4.m128_f32[1] = RotationAxis.y;
					m4.m128_f32[2] = RotationAxis.z;
					m4.m128_f32[3] = 1;
					// Calculate absolute rotation and set it
					auto r_matrix = XMMatrixRotationAxis(m4, RotationAngle);
					skeleton_matrix[CurrentLink.bone_index] = skeleton_matrix[CurrentLink.bone_index] * r_matrix;

					// 旋转极向关节的父关节的 roll 使得极向关节的 Z 轴符合 pole_vector
//					if (pole_effctor_index > 0 && LinkIndex - 1 == pole_effctor_index)
//					{
//						auto axis = NewDir;
//						auto z_dir = GetRowFromBoneIndex(CurrentLink.bone_index, 2).xyz().normalized();
//						auto this_dir = cross(axis, z_dir).normalized();
//						auto taret_dir = cross(axis, pole_vector).normalized();
//
//						float const angle = rotate_vectors(axis, this_dir, taret_dir);
//						XMVECTOR m4;
//						m4.m128_f32[0] = RotationAxis.x;
//						m4.m128_f32[1] = RotationAxis.y;
//						m4.m128_f32[2] = RotationAxis.z;
//						// Calculate absolute rotation and set it
//						auto r_matrix2 = XMMatrixRotationAxis(m4, angle);
//						skeleton_matrix[CurrentLink.bone_index] = skeleton_matrix[CurrentLink.bone_index] * r_matrix2;
//					}


					skeleton_matrix[CurrentLink.bone_index].r[3].m128_f32[0] = CurrentLink.Position.x;
					skeleton_matrix[CurrentLink.bone_index].r[3].m128_f32[1] = CurrentLink.Position.y;
					skeleton_matrix[CurrentLink.bone_index].r[3].m128_f32[2] = CurrentLink.Position.z;
					auto& new_world = skeleton_matrix[CurrentLink.bone_index];
					auto& parent = skeleton_matrix[skeleton_tree_[CurrentLink.bone_index].parent];
					skeleton_locals[CurrentLink.bone_index] = new_world * XMMatrixInverse(nullptr, parent);
					updateChildSkeletonMatrix(CurrentLink.bone_index);
				}

				// 处理末尾 effector 的旋转
				//chain[0].bone->worldTransformMatrix = ik_target->worldTransformMatrix;
				//chain[0].bone->worldTransformMatrix.m[3][0] = chain[0].Position.x;
				//chain[0].bone->worldTransformMatrix.m[3][1] = chain[0].Position.y;
				//chain[0].bone->worldTransformMatrix.m[3][2] = chain[0].Position.z;
				//chain[0].bone->pFirstChild->ComputeWorldTransform(chain[0].bone->worldTransformMatrix);
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
		else if (root_index_ < 0)
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
	//	if (model_.bones.get()[i].grant_weight > 0)
	//	{
	//		if (model_.bones.get()[i].bone_flag & 0x0100)
	//		{
	//			auto tmp = skeleton_locals[i].r[3];
	//			skeleton_locals[i] = model_.bones.get()[i].grant_weight * skeleton_locals[model_.bones.get()[i].grant_parent_index] + (1 - model_.bones.get()[i].grant_weight) * skeleton_locals[i];
	//			skeleton_locals[i].r[3] = tmp;
	//		}
	//		else
	//		{
	//			skeleton_locals[i].r[3] = model_.bones.get()[i].grant_weight * skeleton_locals[model_.bones.get()[i].grant_parent_index].r[3] + (1 - model_.bones.get()[i].grant_weight) * skeleton_locals[i].r[3];
	//		}
	//	}
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

void MotionController::updateInherited()
{
	for (int i = 0; i < bone_count_; ++i)
	{
		const auto &bone = model_.bones.get()[i];
		if (bone.grant_weight > 0)
		{
			if (bone.bone_flag & 0x0100)
			{
				auto tmp = skeleton_locals[i].r[3];
				skeleton_locals[i] = bone.grant_weight * skeleton_locals[bone.grant_parent_index] + (1 - bone.grant_weight) * skeleton_locals[i];
				skeleton_locals[i].r[3] = tmp;
			}
			else
			{
				skeleton_locals[i].r[3] = bone.grant_weight * skeleton_locals[bone.grant_parent_index].r[3] + (1 - bone.grant_weight) * skeleton_locals[i].r[3];
			}
		}
		updateChildSkeletonMatrix(i);
	}
}

void MotionController::resetLocals()
{
	auto model_bone_arr = model_.bones.get();
	for (int i = 0; i < bone_count_; ++i)
	{
		if (model_bone_arr[i].parent_index >= 0)
		{
			skeleton_locals[i] = MathUtil::compose(model_bone_arr[i].position - model_bone_arr[model_bone_arr[i].parent_index].position);
		}
		else
			skeleton_locals[i] = MathUtil::compose(model_bone_arr[i].position);
	}
}

//
//void MotionController::dumpSkeletonTree(std::string file_name)
//{
//	pugi::xml_document dump_doc;
//	auto root_node = dump_doc.append_child(pugi::node_element);
//	root_node.set_name("root");
//	dumpChildSkeletonNodes(root_index_, root_node);
//
//	dump_doc.save_file(file_name.c_str());
//	exit(1);
//}
//
//void MotionController::dumpChildSkeletonNodes(int bone_index, pugi::xml_node &xmlnode)
//{
//	auto &this_bone = model_.bones.get()[bone_index];
//	auto this_node = xmlnode.append_child("node");
//	auto identifier_node = this_node.append_child("identifier").text() = "\tidentifier_text\t";
//	//identifier_node.set_name("identifier");
//	//identifier_node.set_value("test");
//	auto transform_node = this_node.append_child("transform");
//	pugi::char_t buffer[MAX_ROW_CHAR_COUNT];
//	auto translation = skeleton_locals[bone_index].r[3].m128_f32;
//	sprintf_s(buffer, "\t%f %f %f\t", translation[0], translation[1], translation[2]);
//	transform_node.append_child("row0").text() = buffer;
//	transform_node.append_child("row1");
//	transform_node.append_child("row2");
//	transform_node.append_child("row3");
//	for (auto child : skeleton_tree_[bone_index].childs)
//	{
//		dumpChildSkeletonNodes(child, this_node);
//	}
//}
