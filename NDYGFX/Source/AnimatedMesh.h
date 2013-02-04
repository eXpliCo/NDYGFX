/*!
	Class for doing morphanimation. Morphing is done for positions, 
	texture coordinates, normals and colors of the two keyframes.
	Requirements: 
		Atleast two keyframes to morph/interpolate.
		The number of vertices must match between all keyframes.
		Same goes with the number of faces, attributes, materials, textures and shaders.
*/

#pragma once

#include "Mesh.h"
#include "iAnimatedMesh.h"


#pragma warning ( push )
#pragma warning ( disable : 4250 )

class KeyFrame
{
	public:
		int time;
		MaloW::Array<MeshStrip*>* strips;

		KeyFrame() { this->time = 0; this->strips = new MaloW::Array<MeshStrip*>(); }
		virtual ~KeyFrame()
		{
			if(this->strips)
			{
				while(this->strips->size() > 0)
					delete this->strips->getAndRemove(0);

				delete this->strips;
			}
		}
		KeyFrame(const KeyFrame* origObj)
		{
			this->time = origObj->time;
			this->strips = new MaloW::Array<MeshStrip*>();
			for(int i = 0; i < origObj->strips->size(); i++)
			{
				this->strips->add(new MeshStrip(origObj->strips->get(i)));
			}
		}
};


class AnimatedMesh : public Mesh, public virtual iAnimatedMesh
{
	private:
		unsigned int				mNrOfTimesLooped;
		bool						mLoopNormal;
		bool						mLoopSeamless;
		float						mCurrentTime;
		MaloW::Array<KeyFrame*>*	mKeyFrames;

	public:
		AnimatedMesh(D3DXVECTOR3 pos);
		virtual ~AnimatedMesh();

		MaloW::Array<KeyFrame*>*	GetKeyFrames()			const { return this->mKeyFrames; }

		/*! Returns the 2 keyframes to interpolate with value t[0,1] through the parameters depending on the current time. */
		/*!	Note that currentTime is expected to be in milliseconds. Returns NULL if there's no keyframes loaded. */
		/*! Also note that this function can most likely be optimized. */
		void GetCurrentKeyFrames(KeyFrame** one, KeyFrame** two, float& t);

		/* ! Returns the strips of the second mesh currently being used. */
		virtual MaloW::Array<MeshStrip*>* GetStrips();

		/*! Set the current time in milliseconds. This determines what key frames to interpolate. Must be called every frame for animation to work. */
		virtual void SetCurrentTime(float currentTime);
	
		/*! Load the keyframes from file. Input is expected to be "'filename'.ani". */
		virtual bool LoadFromFile(string file);

		// iAnimatedMesh interface functions

		virtual unsigned int	GetNrOfTimesLooped()	const { return this->mNrOfTimesLooped; }
		virtual bool			IsLooping()				const { return this->mLoopNormal || this->mLoopSeamless; }
		virtual bool			IsLoopingNormal()		const { return this->mLoopNormal; }
		virtual bool			IsLoopingSeamless()		const { return this->mLoopSeamless; }
		/*! Prevents looping. */
		virtual void NoLooping();
		/*! Loops by returning to the first keyframe when last keyframe is reached. Note that this kind of looping is not seamless. */
		virtual void LoopNormal();
		/*! Loops by adding the first keyframe as the last keyframe to prevent seamed(normal) looping */
		virtual void LoopSeamless(); 
};

#pragma warning ( pop )