// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __I_SCENE_NODE_H_INCLUDED__
#define __I_SCENE_NODE_H_INCLUDED__

#include "IUnknown.h"
#include "ISceneNodeAnimator.h"
#include "ITriangleSelector.h"
#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "irrList.h"

namespace irr
{
namespace scene
{

	class ISceneManager;

	//! Scene node interface.
	/** A scene node is a node in the hirachical scene graph. Every scene node may have children,
	which are other scene nodes. Children move relative the their parents position. If the parent of a node is not
	visible, its children won't be visible too. In this way, it is for example easily possible
	to attach a light to a moving car, or to place a walking character on a moving platform
	on a moving ship. */
	class ISceneNode : public IUnknown
	{
	public:

		//! Constructor
		ISceneNode(	ISceneNode* parent, ISceneManager* mgr, s32 id=-1,
					const core::vector3df& position = core::vector3df(0,0,0),
					const core::vector3df& rotation = core::vector3df(0,0,0),
					const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
			: IsVisible(true), ID(id), Parent(parent), SceneManager(mgr),
				AutomaticCullingEnabled(true), DebugDataVisible(false),
				TriangleSelector(0), RelativeTranslation(position),
				RelativeRotation(rotation), RelativeScale(scale)

		{
			if (Parent)
				Parent->addChild(this);

			updateAbsolutePosition();			
		}



		//! Destructor
		virtual ~ISceneNode()
		{
			// delete all children
			core::list<ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
				(*it)->drop();

			// delete all animators
			core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
				for (; ait != Animators.end(); ++ait)
					(*ait)->drop();

			if (TriangleSelector)
				TriangleSelector->drop();
		}


		//! This method is called just before the rendering process of the whole scene.
		//! Nodes may register themselves in the render pipeline during this call,
		//! precalculate the geometry which should be renderered, and prevent their
		//! children from being able to register them selfes if they are clipped by simply
		//! not calling their OnPreRender-Method.
		virtual void OnPreRender() 
		{
			if (IsVisible)
			{
				core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
					(*it)->OnPreRender();
			}
		}


		//! OnPostRender() is called just after rendering the whole scene.
		//! Nodes may calculate or store animations here, and may do other useful things,
		//! dependent on what they are.
		//! \param timeMs: Current time in milli seconds.
		virtual void OnPostRender(u32 timeMs)
		{
			if (IsVisible)
			{
				// animate this node with all animators

				core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
				for (; ait != Animators.end(); ++ait)
					(*ait)->animateNode(this, timeMs);

				// update absolute position
				updateAbsolutePosition();

				// perform the post render process on all children
				
				core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
					(*it)->OnPostRender(timeMs);
			}
		}


		//! Renders the node.
		virtual void render() = 0;


		//! Returns the name of the node.
		//! \return Returns name as wide character string.
		virtual const wchar_t* getName() const
		{
			return Name.c_str();
		}


		//! Sets the name of the node.
		//! \param name: New name of the scene node.
		virtual void setName(const wchar_t* name)
		{
			Name = name;
		}


		//! Returns the axis aligned, not transformed bounding box of this node.
		//! This means that if this node is a animated 3d character, moving in a room,
		//! the bounding box will always be around the origin. To get the box in
		//! real world coordinates, just transform it with the matrix you receive with
		//! getAbsoluteTransformation() or simply use getTransformedBoundingBox(),
		//! which does the same.
		virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;


		//! Returns the axis aligned, transformed and animated absolute bounding box
		//! of this node.
		core::aabbox3d<f32> getTransformedBoundingBox()
		{
			core::aabbox3d<f32> box = getBoundingBox();
			AbsoluteTransformation.transformBox(box);
            return box;
		}
		

		//! returns the absolute transformation of the node. Is recalculated every OnPostRender()-call.
		core::matrix4& getAbsoluteTransformation()
		{
			return AbsoluteTransformation;
		}


		//! Returns the relative transformation of the scene node.
		//! The relative transformation is stored internally as 3 vectors:
		//! translation, rotation and scale. To get the relative transformation
		//! matrix, it is calculated from these values.
		//! \return Returns the relative transformation matrix.
		virtual core::matrix4 getRelativeTransformation() const
		{
			core::matrix4 mat;
			mat.setRotationDegrees(RelativeRotation);
			mat.setTranslation(RelativeTranslation);

			if (RelativeScale != core::vector3df(1,1,1))
			{
				core::matrix4 smat;
				smat.setScale(RelativeScale);
				mat *= smat;
			}

			return mat;
		}


		//! Returns true if the node is visible. This is only an option, set by the user and has
		//! nothing to do with geometry culling
		virtual bool isVisible()
		{
			return IsVisible;
		}


		//! Sets if the node should be visible or not. All children of this node won't be visible too.
		virtual void setVisible(bool isVisible)
		{
			IsVisible = isVisible;
		}


		//! Returns the id of the scene node. This id can be used to identify the node.
		virtual s32 getID()
		{
			return ID;
		}


		//! sets the id of the scene node. This id can be used to identify the node.
		virtual void setID(s32 id)
		{
			ID = id;
		}


		//! Adds a child to this scene node. If the scene node already
		//! has got a parent, it is removed from there as child.
		virtual void addChild(ISceneNode* child)
		{
			if (child)
			{
				child->grab();
				child->remove(); // remove from old parent				
				Children.push_back(child);
				child->Parent = this;				
			}
		}


		//! Removes a child from this scene node.
		//! \return Returns true if the child could be removed, and false if not.
		virtual bool removeChild(ISceneNode* child)
		{
			core::list<ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
				if ((*it) == child)
				{
					(*it)->drop();
					Children.erase(it);
					return true;
				}

			return false;
		}


		//! Removes all children of this scene node
		virtual void removeAll()
		{
			core::list<ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
				(*it)->drop();

			Children.clear();
		}


		//! Removes this scene node from the scene, deleting it.
		virtual void remove()
		{
			if (Parent)
				Parent->removeChild(this);
		}


		//! Adds an animator which should animate this node.
		virtual void addAnimator(ISceneNodeAnimator* animator)
		{
			if (animator)
			{
				Animators.push_back(animator);
				animator->grab();
			}
		}



		//! Removes an animator from this scene node.
		virtual void removeAnimator(ISceneNodeAnimator* animator)
		{
			core::list<ISceneNodeAnimator*>::Iterator it = Animators.begin();
			for (; it != Animators.end(); ++it)
				if ((*it) == animator)
				{
					(*it)->drop();
					Animators.erase(it);
					return;
				}
		}


		//! Removes all animators from this scene node.
		virtual void removeAnimators()
		{
			core::list<ISceneNodeAnimator*>::Iterator it = Animators.begin();
			for (; it != Animators.end(); ++it)
				(*it)->drop();

			Animators.clear();	
		}


		//! Returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		//! \param i: Zero based index i. The maximal value for this may be getMaterialCount() - 1.
		//! \return Returns the material of that index.
		virtual video::SMaterial& getMaterial(s32 i)
		{
			return *((video::SMaterial*)0);
		}


		//! Returns amount of materials used by this scene node.
		//! \return Returns current count of materials used by this scene node.
		virtual s32 getMaterialCount()
		{
			return 0;
		}


		//! Sets all material flags at once to a new value. Helpful for
		//! example, if you want to be the the whole mesh to be lighted by 
		//! \param flag: Which flag of all materials to be set.
		//! \param newvalue: New value of the flag.
		void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
		{
			for (s32 i=0; i<getMaterialCount(); ++i)
				getMaterial(i).Flags[flag] = newvalue;
		}


		//! Sets the texture of the specified layer in all materials of this
		//! scene node to the new texture.
		//! \param textureLayer: Layer of texture to be set. Must be a value greater or
		//! equal than 0 and smaller than MATERIAL_MAX_TEXTURES.
		//! \param texture: Texture to be used.
		void setMaterialTexture(s32 textureLayer, video::ITexture* texture)
		{
			if (textureLayer<0 || textureLayer>= video::MATERIAL_MAX_TEXTURES)
				return;

			for (s32 i=0; i<getMaterialCount(); ++i)
				getMaterial(i).Textures[textureLayer] = texture;
		}

			
		//! Sets the material type of all materials s32 this scene node
		//! to a new material type.
		//! \param newType: New type of material to be set.
		void setMaterialType(video::E_MATERIAL_TYPE newType)
		{
			for (s32 i=0; i<getMaterialCount(); ++i)
				getMaterial(i).MaterialType = newType;
		}	


		//! Gets the scale of the scene node. 
		//! \return Returns the scale of the scene node.
		virtual core::vector3df getScale() const
		{
			return RelativeScale;
		}


		//! Sets the scale of the scene node. 
		//! \param scale: New scale of the node
		virtual void setScale(const core::vector3df& scale)
		{
			RelativeScale = scale;
		}


		//! Sets the rotation of the node. Note that this is
		//! the relative rotation of the node.
		//! \return Current relative rotation of the scene node.
		virtual const core::vector3df getRotation() const
		{
			return RelativeRotation;
		}


		//! Sets the rotation of the node. This only modifies
		//! the relative rotation of the node.
		//! \param roation: New rotation of the node in degrees.
		virtual void setRotation(const core::vector3df& rotation)
		{
			RelativeRotation = rotation;
		}


		//! Gets the position of the node. Note that the position is
		//! relative to the parent.
		//! \return Returns the current position of the node relative to the parent.
		virtual const core::vector3df getPosition() const
		{
			return RelativeTranslation;
		}


		//! Sets the position of the node. Note that the position is
		//! relative to the parent.
		//! \param newpos: New relative postition of the scene node.
		virtual void setPosition(const core::vector3df& newpos)
		{
			RelativeTranslation = newpos;
		}


		//! Gets the abolute position of the node. The position is absolute.
		//! \return Returns the current absolute position of the scene node.
		virtual core::vector3df getAbsolutePosition() const
		{
			core::vector3df pos(0.0f, 0.0f, 0.0f);
			AbsoluteTransformation.transformVect(pos);
			return pos;
		}


		//! Enables or disables automatic culling based on the bounding
		//! box. Automatic culling is enabled by default. Note that not
		//! all SceneNodes support culling (the billboard scene node for example)
		//! and that some nodes always cull their geometry because it is their
		//! only reason for existance, for example the OctreeSceneNode.
		//! \param enabled: If true, automatic culling is enabled.
		//! If false, it is disabled.
		void setAutomaticCulling(bool enabled)
		{
			AutomaticCullingEnabled = enabled;
		}


		//! Gets the automatic culling state. 
		//! \return The node is culled based on its bounding box if this method
		//! returns true, otherwise no culling is performed.
		bool getAutomaticCulling() const
		{
			return AutomaticCullingEnabled;
		}


		//! Sets if debug data like bounding boxes should be drawed.
		//! Please note that not all scene nodes support this feature.
		void setDebugDataVisible(bool visible)
		{
			DebugDataVisible = visible;
		}

		//! Returns if debug data like bounding boxes are drawed.
		bool isDebugDataVisible()
		{
			return DebugDataVisible;
		}


		//! Returns a const reference to the list of all children.
		const core::list<ISceneNode*>& getChildren() const
		{
			return Children;
		}


		//! Changes the parent of the scene node.
		virtual void setParent(ISceneNode* newParent)
		{
			grab();
			remove();

			Parent = newParent;

			if (Parent)
				Parent->addChild(this);

			drop();
		}


		//! Returns the triangle selector attached to this scene node.
		//! The Selector can be used by the engine for doing collision
		//! detection. You can create a TriangleSelector with 
		//! ISceneManager::createTriangleSelector() or 
		//! ISceneManager::createOctTreeTriangleSelector and set it with
		//! ISceneNode::setTriangleSelector(). If a scene node got no triangle
		//! selector, but collision tests should be done with it, a triangle
		//! selector is created using the bounding box of the scene node.
		//! \return Returns a pointer to the TriangleSelector or NULL, if there
		//! is none.
		virtual ITriangleSelector* getTriangleSelector() const
		{
			return TriangleSelector;
		}


		//! Sets the triangle selector of the scene node. The Selector can be
		//! used by the engine for doing collision detection. You can create a
		//! TriangleSelector with ISceneManager::createTriangleSelector() or 
		//! ISceneManager::createOctTreeTriangleSelector(). Some nodes may
		//! create their own selector by default, so it would be good to
		//! check if there is already a selector in this node by calling
		//! ISceneNode::getTriangleSelector(). 
		//! \param selector: New triangle selector for this scene node.
		virtual void setTriangleSelector(ITriangleSelector* selector) 
		{
			if (TriangleSelector)
				TriangleSelector->drop();

			TriangleSelector = selector;
			TriangleSelector->grab();
		}

		//! updates the absolute position based on the relative and the parents position
		virtual void updateAbsolutePosition()
		{
			if (Parent)
				AbsoluteTransformation = 
					Parent->getAbsoluteTransformation() * getRelativeTransformation();
			else
				AbsoluteTransformation = getRelativeTransformation();
		}

	protected:

		//! name of the scene node.
		core::stringw Name;	

		//! absolute transformation of the node.
		core::matrix4 AbsoluteTransformation; 

		//! relative translation of the scene node.
		core::vector3df RelativeTranslation;

		//! relative rotation of the scene node.
		core::vector3df RelativeRotation;

		//! relative scale of the scene node.
		core::vector3df RelativeScale;

		//! Pointer to the parent
		ISceneNode* Parent;

		//! List of all children of this node
		core::list<ISceneNode*> Children;

		//! List of all children of this node
		core::list<ISceneNodeAnimator*> Animators;

		//! id of the node. 
		s32 ID;

		//! pointer to the scene manager
		ISceneManager* SceneManager;

		//! pointer to the triangleselector
		ITriangleSelector* TriangleSelector;

		//! automatic culling
		bool AutomaticCullingEnabled;

		//! flag if debug data should be drawed, like Bounding Boxes.
		bool DebugDataVisible;		

		//! is the node visible?
		bool IsVisible;
	};

} // end namespace scene
} // end namespace irr

#endif

