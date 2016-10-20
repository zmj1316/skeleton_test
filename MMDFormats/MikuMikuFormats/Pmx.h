#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <DirectXMath.h>
#include "../../DxBase/math3d.h"

namespace pmx
{
	/// �C���f�b�N�X�ݒ�
	class PmxSetting
	{
	public:
		PmxSetting()
			: encoding(0)
			, uv(0)
			, vertex_index_size(0)
			, texture_index_size(0)
			, material_index_size(0)
			, bone_index_size(0)
			, morph_index_size(0)
			, rigidbody_index_size(0)
		{}

		/// �G���R�[�h����
		uint8_t encoding;
		/// �ǉ�UV��
		uint8_t uv;
		/// ���_�C���f�b�N�X�T�C�Y
		uint8_t vertex_index_size;
		/// �e�N�X�`���C���f�b�N�X�T�C�Y
		uint8_t texture_index_size;
		/// �}�e���A���C���f�b�N�X�T�C�Y
		uint8_t material_index_size;
		/// �{�[���C���f�b�N�X�T�C�Y
		uint8_t bone_index_size;
		/// ���[�t�C���f�b�N�X�T�C�Y
		uint8_t morph_index_size;
		/// ���̃C���f�b�N�X�T�C�Y
		uint8_t rigidbody_index_size;
		void Read(std::istream *stream);
	};

	/// ���_�X�L�j���O�^�C�v
	enum class PmxVertexSkinningType : uint8_t
	{
		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF = 3,
		QDEF = 4,
	};

	/// ���_�X�L�j���O
	class PmxVertexSkinning
	{
	public:
		virtual void Read(std::istream *stream, PmxSetting *setting) = 0;
	};

	class PmxVertexSkinningBDEF1 : public PmxVertexSkinning
	{
	public:
		PmxVertexSkinningBDEF1()
			: bone_index(0)
		{}

		int bone_index;
		void Read(std::istream *stresam, PmxSetting *setting);
	};

	class PmxVertexSkinningBDEF2 : public PmxVertexSkinning
	{
	public:
		PmxVertexSkinningBDEF2()
			: bone_index1(0)
			, bone_index2(0)
			, bone_weight(0.0f)
		{}

		int bone_index1;
		int bone_index2;
		float bone_weight;
		void Read(std::istream *stresam, PmxSetting *setting);
	};

	class PmxVertexSkinningBDEF4 : public PmxVertexSkinning
	{
	public:
		PmxVertexSkinningBDEF4()
			: bone_index1(0)
			, bone_index2(0)
			, bone_index3(0)
			, bone_index4(0)
			, bone_weight1(0.0f)
			, bone_weight2(0.0f)
			, bone_weight3(0.0f)
			, bone_weight4(0.0f)
		{}

		int bone_index1;
		int bone_index2;
		int bone_index3;
		int bone_index4;
		float bone_weight1;
		float bone_weight2;
		float bone_weight3;
		float bone_weight4;
		void Read(std::istream *stresam, PmxSetting *setting);
	};

	class PmxVertexSkinningSDEF : public PmxVertexSkinning
	{
	public:
		PmxVertexSkinningSDEF()
			: bone_index1(0)
			, bone_index2(0)
			, bone_weight(0.0f)
		{
			for (int i = 0; i < 3; ++i) {
				sdef_c[i] = 0.0f;
				sdef_r0[i] = 0.0f;
				sdef_r1[i] = 0.0f;
			}
		}

		int bone_index1;
		int bone_index2;
		float bone_weight;
		float sdef_c[3];
		float sdef_r0[3];
		float sdef_r1[3];
		void Read(std::istream *stresam, PmxSetting *setting);
	};

	class PmxVertexSkinningQDEF : public PmxVertexSkinning
	{
	public:
		PmxVertexSkinningQDEF()
			: bone_index1(0)
			, bone_index2(0)
			, bone_index3(0)
			, bone_index4(0)
			, bone_weight1(0.0f)
			, bone_weight2(0.0f)
			, bone_weight3(0.0f)
			, bone_weight4(0.0f)
		{}

		int bone_index1;
		int bone_index2;
		int bone_index3;
		int bone_index4;
		float bone_weight1;
		float bone_weight2;
		float bone_weight3;
		float bone_weight4;
		void Read(std::istream *stresam, PmxSetting *setting);
	};

	/// ���_
	class PmxVertex
	{
	public:
		PmxVertex()
			: edge(0.0f)
		{
			uv[0] = uv[1] = 0.0f;
			for (int i = 0; i < 3; ++i) {
				positon[i] = 0.0f;
				normal[i] = 0.0f;
			}
			for (int i = 0; i < 4; ++i) {
				for (int k = 0; k < 4; ++k) {
					uva[i][k] = 0.0f;
				}
			}
		}

		/// �ʒu
		float positon[3];
		/// �@��
		float normal[3];
		/// �e�N�X�`�����W
		float uv[2];
		/// �ǉ��e�N�X�`�����W
		float uva[4][4];
		/// �X�L�j���O�^�C�v
		PmxVertexSkinningType skinning_type;
		/// �X�L�j���O
		std::unique_ptr<PmxVertexSkinning> skinning;
		/// �G�b�W�{��
		float edge;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	/// �}�e���A��
	class PmxMaterial
	{
	public:
		PmxMaterial()
			: specularlity(0.0f)
			, flag(0)
			, edge_size(0.0f)
			, diffuse_texture_index(0)
			, sphere_texture_index(0)
			, sphere_op_mode(0)
			, common_toon_flag(0)
			, toon_texture_index(0)
			, index_count(0)
		{
			for (int i = 0; i < 3; ++i) {
				specular[i] = 0.0f;
				ambient[i] = 0.0f;
				edge_color[i] = 0.0f;
			}
			for (int i = 0; i < 4; ++i) {
				diffuse[i] = 0.0f;
			}
		}

		/// ���f����
		std::wstring material_name;
		/// ���f���p��
		std::wstring material_english_name;
		/// �����F
		float diffuse[4];
		/// ����F
		float specular[3];
		/// ����x
		float specularlity;
		/// ���F
		float ambient[3];
		/// �`��t���O
		uint8_t flag;
		/// �G�b�W�F
		float edge_color[4];
		/// �G�b�W�T�C�Y
		float edge_size;
		/// �A���x�h�e�N�X�`���C���f�b�N�X
		int diffuse_texture_index;
		/// �X�t�B�A�e�N�X�`���C���f�b�N�X
		int sphere_texture_index;
		/// �X�t�B�A�e�N�X�`�����Z���[�h
		uint8_t sphere_op_mode;
		/// ���L�g�D�[���t���O
		uint8_t common_toon_flag;
		/// �g�D�[���e�N�X�`���C���f�b�N�X
		int toon_texture_index;
		/// ����
		std::wstring memo;
		/// ���_�C���f�b�N�X��
		int index_count;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	/// �����N
	class PmxIkLink
	{
	public:
		PmxIkLink()
			: link_target(0)
			, angle_lock(0)
		{
			for (int i = 0; i < 3; ++i) {
				max_radian[i] = 0.0f;
				min_radian[i] = 0.0f;
			}
		}

		/// �����N�{�[���C���f�b�N�X
		int link_target;
		/// �p�x����
		uint8_t angle_lock;
		/// �ő吧���p�x
		float max_radian[3];
		/// �ŏ������p�x
		float min_radian[3];
		void Read(std::istream *stream, PmxSetting *settingn);
	};

	/// �{�[��
	class PmxBone
	{
	public:
		PmxBone()
			: parent_index(0)
			, level(0)
			, bone_flag(0)
			, target_index(0)
			, grant_parent_index(0)
			, grant_weight(0.0f)
			, key(0)
			, ik_target_bone_index(0)
			, ik_loop(0)
			, ik_loop_angle_limit(0.0f)
			, ik_link_count(0)
		{
			for (int i = 0; i < 3; ++i) {
				offset[i] = 0.0f;
				lock_axis_orientation[i] = 0.0f;
				local_axis_x_orientation[i] = 0.0f;
				local_axis_y_orientation[i] = 0.0f;
			}
		}

		/// �{�[����
		std::wstring bone_name;
		/// �{�[���p��
		std::wstring bone_english_name;
		/// �ʒu
		vec3 position;
		/// �e�{�[���C���f�b�N�X
		int parent_index;
		/// �K�w
		int level;
		/// �{�[���t���O
		uint16_t bone_flag;
		/// ���W�I�t�Z�b�g(has Target)
		float offset[3];
		/// �ڑ���{�[���C���f�b�N�X(not has Target)
		int target_index;
		/// �t�^�e�{�[���C���f�b�N�X
		int grant_parent_index;
		/// �t�^��
		float grant_weight;
		/// �Œ莲�̕���
		float lock_axis_orientation[3];
		/// ���[�J������X������
		float local_axis_x_orientation[3];
		/// ���[�J������Y������
		float local_axis_y_orientation[3];
		/// �O���e�ό`��key�l
		int key;
		/// IK�^�[�Q�b�g�{�[��
		int ik_target_bone_index;
		/// IK���[�v��
		int ik_loop;
		/// IK���[�v�v�Z���̊p�x����(���W�A��)
		float ik_loop_angle_limit;
		/// IK�����N��
		int ik_link_count;
		/// IK�����N

		std::unique_ptr<PmxIkLink []> ik_links;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	enum class MorphType : uint8_t
	{
		Group = 0,
		Vertex = 1,
		Bone = 2,
		UV = 3,
		AdditionalUV1 = 4,
		AdditionalUV2 = 5,
		AdditionalUV3 = 6,
		AdditionalUV4 = 7,
		Matrial = 8,
		Flip = 9,
		Implus = 10,
	};

	enum class MorphCategory : uint8_t
	{
		ReservedCategory = 0,
		Eyebrow = 1,
		Eye = 2,
		Mouth = 3,
		Other = 4,
	};

	class PmxMorphOffset
	{
	public:
		void virtual Read(std::istream *stream, PmxSetting *setting) = 0;
	};

	class PmxMorphVertexOffset : public PmxMorphOffset
	{
	public:
		PmxMorphVertexOffset()
			: vertex_index(0)
		{
			for (int i = 0; i < 3; ++i) {
				position_offset[i] = 0.0f;
			}
		}
		int vertex_index;
		float position_offset[3];
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	class PmxMorphUVOffset : public PmxMorphOffset
	{
	public:
		PmxMorphUVOffset()
			: vertex_index(0)
		{
			for (int i = 0; i < 4; ++i) {
				uv_offset[i] = 0.0f;
			}
		}
		int vertex_index;
		float uv_offset[4];
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	class PmxMorphBoneOffset : public PmxMorphOffset
	{
	public:
		PmxMorphBoneOffset()
			: bone_index(0)
		{
			for (int i = 0; i < 3; ++i) {
				translation[i] = 0.0f;
			}
			for (int i = 0; i < 4; ++i) {
				rotation[i] = 0.0f;
			}
		}
		int bone_index;
		float translation[3];
		float rotation[4];
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	class PmxMorphMaterialOffset : public PmxMorphOffset
	{
	public:
		PmxMorphMaterialOffset()
			: specularity(0.0f)
			, edge_size(0.0f)
		{
			for (int i = 0; i < 3; ++i) {
				specular[i] = 0.0f;
				ambient[i] = 0.0f;
			}
			for (int i = 0; i < 4; ++i) {
				diffuse[i] = 0.0f;
				edge_color[i] = 0.0f;
				texture_argb[i] = 0.0f;
				sphere_texture_argb[i] = 0.0f;
				toon_texture_argb[i] = 0.0f;
			}
		}
		int material_index;
		uint8_t offset_operation;
		float diffuse[4];
		float specular[3];
		float specularity;
		float ambient[3];
		float edge_color[4];
		float edge_size;
		float texture_argb[4];
		float sphere_texture_argb[4];
		float toon_texture_argb[4];
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	class PmxMorphGroupOffset : public PmxMorphOffset
	{
	public:
		PmxMorphGroupOffset()
			: morph_index(0)
			, morph_weight(0.0f)
		{}
		int morph_index;
		float morph_weight;
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	class PmxMorphFlipOffset : public PmxMorphOffset
	{
	public:
		PmxMorphFlipOffset()
			: morph_index(0)
			, morph_value(0.0f)
		{}
		int morph_index;
		float morph_value;
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	class PmxMorphImplusOffset : public PmxMorphOffset
	{
	public:
		PmxMorphImplusOffset()
			: rigid_body_index(0)
			, is_local(0)
		{
			for (int i = 0; i < 3; ++i) {
				velocity[i] = 0.0f;
				angular_torque[i] = 0.0f;
			}
		}
		int rigid_body_index;
		uint8_t is_local;
		float velocity[3];
		float angular_torque[3];
		void Read(std::istream *stream, PmxSetting *setting) override;
	};

	/// ���[�t
	class PmxMorph
	{
	public:
		PmxMorph()
			: offset_count(0)
		{
		}
		/// ���[�t��
		std::wstring morph_name;
		/// ���[�t�p��
		std::wstring morph_english_name;
		/// �J�e�S��
		MorphCategory category;
		/// ���[�t�^�C�v
		MorphType morph_type;
		/// �I�t�Z�b�g��
		int offset_count;
		/// ���_���[�t�z��
		std::unique_ptr<PmxMorphVertexOffset []> vertex_offsets;
		/// UV���[�t�z��
		std::unique_ptr<PmxMorphUVOffset []> uv_offsets;
		/// �{�[�����[�t�z��
		std::unique_ptr<PmxMorphBoneOffset []> bone_offsets;
		/// �}�e���A�����[�t�z��
		std::unique_ptr<PmxMorphMaterialOffset []> material_offsets;
		/// �O���[�v���[�t�z��
		std::unique_ptr<PmxMorphGroupOffset []> group_offsets;
		/// �t���b�v���[�t�z��
		std::unique_ptr<PmxMorphFlipOffset []> flip_offsets;
		/// �C���p���X���[�t�z��
		std::unique_ptr<PmxMorphImplusOffset []> implus_offsets;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	/// �g���v�f
	class PmxFrameElement
	{
	public:
		PmxFrameElement()
			: element_target(0)
			, index(0)
		{
		}
		/// �v�f�Ώ�
		uint8_t element_target;
		/// �v�f�ΏۃC���f�b�N�X
		int index;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	/// �\���g
	class PmxFrame
	{
	public:
		PmxFrame()
			: frame_flag(0)
			, element_count(0)
		{
		}
		/// �g��
		std::wstring frame_name;
		/// �g�p��
		std::wstring frame_english_name;
		/// ����g�t���O
		uint8_t frame_flag;
		/// �g���v�f��
		int element_count;
		/// �g���v�f�z��
		std::unique_ptr<PmxFrameElement []> elements;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	class PmxRigidBody
	{
	public:
		PmxRigidBody()
			: target_bone(0)
			, group(0)
			, mask(0)
			, shape(0)
			, mass(0.0f)
			, move_attenuation(0.0f)
			, rotation_attenuation(0.0f)
			, repulsion(0.0f)
			, friction(0.0f)
			, physics_calc_type(0)
		{
			for (int i = 0; i < 3; ++i) {
				size[i] = 0.0f;
				position[i] = 0.0f;
				orientation[i] = 0.0f;
			}
		}
		/// ���̖�
		std::wstring girid_body_name;
		/// ���̉p��
		std::wstring girid_body_english_name;
		/// �֘A�{�[���C���f�b�N�X
		int target_bone;
		/// �O���[�v
		uint8_t group;
		/// �}�X�N
		uint16_t mask;
		/// �`��
		uint8_t shape;
		float size[3];
		float position[3];
		float orientation[3];
		float mass;
		float move_attenuation;
		float rotation_attenuation;
		float repulsion;
		float friction;
		uint8_t physics_calc_type;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	enum class PmxJointType : uint8_t
	{
		Generic6DofSpring = 0,
		Generic6Dof = 1,
		Point2Point = 2,
		ConeTwist = 3,
		Slider = 5,
		Hinge = 6
	};

	class PmxJointParam
	{
	public:
		PmxJointParam()
			: rigid_body1(0)
			, rigid_body2(0)
		{
			for (int i = 0; i < 3; ++i) {
				position[i] = 0.0f;
				orientaiton[i] = 0.0f;
				move_limitation_min[i] = 0.0f;
				move_limitation_max[i] = 0.0f;
				rotation_limitation_min[i] = 0.0f;
				rotation_limitation_max[i] = 0.0f;
				spring_move_coefficient[i] = 0.0f;
				spring_rotation_coefficient[i] = 0.0f;
			}
		}
		int rigid_body1;
		int rigid_body2;
		float position[3];
		float orientaiton[3];
		float move_limitation_min[3];
		float move_limitation_max[3];
		float rotation_limitation_min[3];
		float rotation_limitation_max[3];
		float spring_move_coefficient[3];
		float spring_rotation_coefficient[3];
		void Read(std::istream *stream, PmxSetting *setting);
	};

	class PmxJoint
	{
	public:
		std::wstring joint_name;
		std::wstring joint_english_name;
		PmxJointType joint_type;
		PmxJointParam param;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	enum PmxSoftBodyFlag : uint8_t
	{
		BLink = 0x01,
		Cluster = 0x02,
		Link = 0x04
	};

	class PmxAncherRigidBody
	{
	public:
		PmxAncherRigidBody()
			: related_rigid_body(0)
			, related_vertex(0)
			, is_near(false)
		{}
		int related_rigid_body;
		int related_vertex;
		bool is_near;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	class PmxSoftBody
	{
	public:
		PmxSoftBody()
			: shape(0)
			, target_material(0)
			, group(0)
			, mask(0)
			, blink_distance(0)
			, cluster_count(0)
			, mass(0.0)
			, collisioni_margin(0.0)
			, aero_model(0)
			, VCF(0.0f)
			, DP(0.0f)
			, DG(0.0f)
			, LF(0.0f)
			, PR(0.0f)
			, VC(0.0f)
			, DF(0.0f)
			, MT(0.0f)
			, CHR(0.0f)
			, KHR(0.0f)
			, SHR(0.0f)
			, AHR(0.0f)
			, SRHR_CL(0.0f)
			, SKHR_CL(0.0f)
			, SSHR_CL(0.0f)
			, SR_SPLT_CL(0.0f)
			, SK_SPLT_CL(0.0f)
			, SS_SPLT_CL(0.0f)
			, V_IT(0)
			, P_IT(0)
			, D_IT(0)
			, C_IT(0)
			, LST(0.0f)
			, AST(0.0f)
			, VST(0.0f)
			, anchor_count(0)
			, pin_vertex_count(0)
		{}
		std::wstring soft_body_name;
		std::wstring soft_body_english_name;
		uint8_t shape;
		int target_material;
		uint8_t group;
		uint16_t mask;
		PmxSoftBodyFlag flag;
		int blink_distance;
		int cluster_count;
		float mass;
		float collisioni_margin;
		int aero_model;
		float VCF;
		float DP;
		float DG;
		float LF;
		float PR;
		float VC;
		float DF;
		float MT;
		float CHR;
		float KHR;
		float SHR;
		float AHR;
		float SRHR_CL;
		float SKHR_CL;
		float SSHR_CL;
		float SR_SPLT_CL;
		float SK_SPLT_CL;
		float SS_SPLT_CL;
		int V_IT;
		int P_IT;
		int D_IT;
		int C_IT;
		float LST;
		float AST;
		float VST;
		int anchor_count;
		std::unique_ptr<PmxAncherRigidBody []> anchers;
		int pin_vertex_count;
		std::unique_ptr<int []> pin_vertices;
		void Read(std::istream *stream, PmxSetting *setting);
	};

	/// PMX���f��
	class PmxModel
	{
	public:
		PmxModel()
			: version(0.0f)
			, vertex_count(0)
			, index_count(0)
			, texture_count(0)
			, material_count(0)
			, bone_count(0)
			, morph_count(0)
			, frame_count(0)
			, rigid_body_count(0)
			, joint_count(0)
			, soft_body_count(0)
		{}

		/// �o�[�W����
		float version;
		/// �ݒ�
		PmxSetting setting;
		/// ���f����
		std::wstring model_name;
		/// ���f���p��
		std::wstring model_english_name;
		/// �R�����g
		std::wstring model_comment;
		/// �p��R�����g
		std::wstring model_english_commnet;
		/// ���_��
		int vertex_count;
		/// ���_�z��
		std::unique_ptr<PmxVertex []> vertices;
		/// �C���f�b�N�X��
		int index_count;
		/// �C���f�b�N�X�z��
		std::unique_ptr<int []> indices;
		/// �e�N�X�`����
		int texture_count;
		/// �e�N�X�`���z��
		std::unique_ptr< std::wstring []> textures;
		/// �}�e���A����
		int material_count;
		/// �}�e���A��
		std::unique_ptr<PmxMaterial []> materials;
		/// �{�[����
		int bone_count;
		/// �{�[���z��
		std::unique_ptr<PmxBone []> bones;
		/// ���[�t��
		int morph_count;
		/// ���[�t�z��
		std::unique_ptr<PmxMorph []> morphs;
		/// �\���g��
		int frame_count;
		/// �\���g�z��
		std::unique_ptr<PmxFrame [] > frames;
		/// ���̐�
		int rigid_body_count;
		/// ���̔z��
		std::unique_ptr<PmxRigidBody []> rigid_bodies;
		/// �W���C���g��
		int joint_count;
		/// �W���C���g�z��
		std::unique_ptr<PmxJoint []> joints;
		/// �\�t�g�{�f�B��
		int soft_body_count;
		/// �\�t�g�{�f�B�z��
		std::unique_ptr<PmxSoftBody []> soft_bodies;
		/// ���f��������
		void Init();
		/// ���f���ǂݍ���
		void Read(std::istream *stream);
		///// �t�@�C�����烂�f���̓ǂݍ���
		//static std::unique_ptr<PmxModel> ReadFromFile(const char *filename);
		///// ���̓X�g���[�����烂�f���̓ǂݍ���
		//static std::unique_ptr<PmxModel> ReadFromStream(std::istream *stream);
	};
}
