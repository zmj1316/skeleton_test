#pragma once
#include "../MMDFormats/MikuMikuFormats/Vmd.h"
#include "../MMDFormats/MikuMikuFormats/Pmx.h"
#include <list>
#include "mathUtil.h"
#include "GeometryData.h"
class MotionController
{
public:
	MotionController(): ctime(0),root_index_(-1)
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

	void updateMorphAnimation(const std::vector<MyMeshData::Vertex>&, std::vector<MyMeshData::Vertex>&);
	void updateBoneAnimation();
	void updateIK();
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
	int root_index_;
	void allocVectors();
	void buildSkeletonTree();
	void updateChildSkeletonMatrix(int i);
};
