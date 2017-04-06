#pragma once
#include "../MMDFormats/MikuMikuFormats/Vmd.h"
#include "../MMDFormats/MikuMikuFormats/Pmx.h"
#include <list>
#include "mathUtil.h"
#include "GeometryData.h"

const int MAX_ROW_CHAR_COUNT = 50;

namespace pugi
{
	class xml_node;
}

class MotionController
{
public:
	MotionController(): ctime(0), root_index_(1)
	{
	}

	~MotionController();
	bool LoadModel(std::wstring model_base, std::wstring pmx_name, std::wstring vmd_name);

	void updateMatrix();

	const pmx::PmxModel& getModel() const
	{
		return model_;
	}

	std::unique_ptr<vmd::VmdMotion>::pointer getMotion() const
	{
		return motion_.get();
	}

	std::wstring getBasePath() const
	{
		return model_base_;
	}

	void updateMorphAnimation(const std::vector<MyMeshData::Vertex> &, std::vector<MyMeshData::Vertex> &);
	void updateBoneAnimation();
	void updateIK();
	void updateIK_FABRIK();
	std::vector<DirectX::XMMATRIX> skeleton_matrix;

	struct TreeNode
	{
		int parent = -1;
		std::vector<int> childs;
	};

	void advanceTime()
	{
		ctime += 1;
	}

	void dumpSkeletonTree(std::string file_name);

	int bone_count_;
	int morph_count_;
private:
	std::wstring model_base_;

	pmx::PmxModel model_;
	std::unique_ptr<vmd::VmdMotion> motion_;
	std::vector<std::list<vmd::VmdBoneFrame>> bone_key_frames_;
	std::vector<std::list<vmd::VmdBoneFrame>::iterator> bone_key_frames_itr_;

	std::vector<vec3> skeleton_position;
	std::vector<DirectX::XMMATRIX> inv_position;
	std::vector<vec4> skeleton_rotation;
	std::vector<DirectX::XMMATRIX> skeleton_locals;
	std::vector<TreeNode> skeleton_tree_;

	std::vector<std::list<vmd::VmdFaceFrame>> morph_key_frames_;
	std::vector<std::list<vmd::VmdFaceFrame>::iterator> morph_key_frames_itr_;
	std::vector<float> morph_weights_;

	int ctime;
	int root_index_ = 1;
	void allocVectors();
	void buildSkeletonTree();
	void updateChildSkeletonMatrix(int i);
	void updateInherited();


	void solveFABRIK(int ik_effector,vec3 ik_position,int chain_length);
	void resetLocals();
	void dumpChildSkeletonNodes(int bone_index, pugi::xml_node &xmlnode);

	vec4 GetRowFromBoneIndex(int index, int row)
	{
		const auto &mat = skeleton_matrix[index];
		return vec4(mat.r[row]);
	}

	vec3 GetPositionFromBoneIndex(int index)
	{
		return GetRowFromBoneIndex(index, 3).xyz();
	}

	static float rotate_vectors(vec3 axis, vec3 from, vec3 to)
	{
		auto cosine = dot(from, to);
		if (cosine > 1.0f)
			cosine = 1.0f;
		auto angle = acos(cosine);
		auto c = cross(from, to);
		if (dot(c, axis) < 0)
			angle = -angle;
		return angle;
	}

	struct FABRIKChainLink
	{
	public:
		/** Position of bone in component space. */
		vec3 Position;

		/** Distance to its parent link. */
		float Length;

		/** Bone in SkeletalMesh */
		int bone_index;

		FABRIKChainLink(const vec3 &InPosition, const float &InLength, int InBone)
			: Position(InPosition)
			, Length(InLength)
			, bone_index(InBone)
		{
		}
	};
};
