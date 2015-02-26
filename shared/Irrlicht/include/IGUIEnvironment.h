// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_ENVIRONMENT_H_INCLUDED__
#define __I_GUI_ENVIRONMENT_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IGUISkin.h"
#include "rect.h"
#include "EMessageBoxFlags.h"
#include "IEventReceiver.h"
#include "IXMLReader.h"
#include "path.h"

//by stone
namespace irr
{
	class IOSOperator;
	class IEventReceiver;

	namespace io
	{
		class IXMLWriter;
		class IReadFile;
		class IWriteFile;
		class IFileSystem;
	} // end namespace io
	namespace video
	{
		class IVideoDriver;
		class ITexture;
	} // end namespace video

namespace gui
{

class IGUIElement;
class IGUIFont;
class IGUISpriteBank;
class IGUIScrollBar;
class IGUIImage;
class IGUIMeshViewer;
class IGUICheckBox;
class IGUIListBox;
class IGUITreeView;
class IGUIImageList;
class IGUIFileOpenDialog;
class IGUIColorSelectDialog;
class IGUIInOutFader;
class IGUIStaticText;
class IGUIEditBox;
class IGUISpinBox;
class IGUITabControl;
class IGUITab;
class IGUITable;
class IGUIContextMenu;
class IGUIComboBox;
class IGUIToolBar;
class IGUIButton;
class IGUIWindow;
class IGUIElementFactory;

//! GUI Environment. Used as factory and manager of all other GUI elements.
/** \par This element can create the following events of type EGUI_EVENT_TYPE (which are passed on to focused sub-elements):
\li EGET_ELEMENT_FOCUS_LOST
\li EGET_ELEMENT_FOCUSED
\li EGET_ELEMENT_LEFT
\li EGET_ELEMENT_HOVERED
*/
class IGUIEnvironment : public virtual IReferenceCounted
{
public:

	//! Draws all gui elements by traversing the GUI environment starting at the root node.
	virtual void drawAll() = 0;
    
    //! Returns the current video driver.
	virtual video::IVideoDriver* getVideoDriver() const = 0;
    
	//! Returns the file system.
	virtual io::IFileSystem* getFileSystem() const = 0;
    
	//! returns a pointer to the OS operator
	virtual IOSOperator* getOSOperator() const = 0;
    
	//! Posts an input event to the environment.
	virtual bool postEventFromUser(const SEvent& event) = 0;
    
	//! This sets a new event receiver for gui events.
	virtual void setUserEventReceiver(IEventReceiver* evr) = 0;
    
    //! Removes all elements from the environment.
	virtual void clear() = 0;
    
	//! Sets the focus to an element.
	virtual bool setFocus(IGUIElement* element) = 0;
    
    //! Removes the focus from an element.
	virtual bool removeFocus(IGUIElement* element) = 0;
    
    //! Returns whether the element has focus
	virtual bool hasFocus(IGUIElement* element, bool checkSubElements=false) const = 0;

	//! Returns the element which holds the focus.
	virtual IGUIElement* getFocus() const = 0;

	//! Returns the element which was last under the mouse cursor
	virtual IGUIElement* getHovered() const = 0;
		
	//! Creates a new GUI Skin based on a template.
	virtual IGUISkin* createSkin(EGUI_SKIN_TYPE type) = 0;
        
    //! Returns pointer to the current gui skin.
	virtual IGUISkin* getSkin() const = 0;

    //! Sets a new GUI Skin
	virtual void setSkin(IGUISkin* skin) = 0;
	
	//! Creates the image list from the given texture.
	virtual IGUIImageList* createImageList( video::ITexture* texture,					core::dimension2d<s32> imageSize, bool useAlphaChannel ) = 0;

	//! Returns pointer to the font with the specified filename.
	virtual IGUIFont* getFont(const io::path& filename) = 0;

	//! Adds an externally loaded font to the font list.
	virtual IGUIFont* addFont(const io::path& name, IGUIFont* font) = 0;

	//! remove loaded font
	virtual void removeFont(IGUIFont* font) = 0;

	//! Returns the default built-in font.
	virtual IGUIFont* getBuiltInFont() const = 0;

	//! Returns pointer to the sprite bank with the specified file name.
	virtual IGUISpriteBank* getSpriteBank(const io::path& filename) = 0;

	//! Adds an empty sprite bank to the manager
	virtual IGUISpriteBank* addEmptySpriteBank(const io::path& name) = 0;
	
	//! Adds a button element.
	virtual IGUIButton* addButton(const core::rect<s32>& rectangle,		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0, const wchar_t* tooltiptext = 0) = 0;

	//! Adds an empty window element.
	virtual IGUIWindow* addWindow(const core::rect<s32>& rectangle, bool modal = false, const wchar_t* text=0, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a modal screen.
	virtual IGUIElement* addModalScreen(IGUIElement* parent) = 0;

	//! Adds a message box.
	virtual IGUIWindow* addMessageBox(const wchar_t* caption, const wchar_t* text=0, bool modal = true, s32 flags = EMBF_OK, IGUIElement* parent=0, s32 id=-1, video::ITexture* image=0) = 0;

	//! Adds a scrollbar.
	virtual IGUIScrollBar* addScrollBar(bool horizontal, const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds an image element.
	virtual IGUIImage* addImage(video::ITexture* image, core::position2d<s32> pos,
		bool useAlphaChannel=true, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds an image element.
	virtual IGUIImage* addImage(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0, bool useAlphaChannel=true) = 0;

	//! Adds a checkbox element.
	virtual IGUICheckBox* addCheckBox(bool checked, const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds a list box element.
	virtual IGUIListBox* addListBox(const core::rect<s32>& rectangle,		IGUIElement* parent=0, s32 id=-1, bool drawBackground=false) = 0;

	//! Adds a tree view element.
	virtual IGUITreeView* addTreeView(const core::rect<s32>& rectangle,		IGUIElement* parent=0, s32 id=-1, bool drawBackground=false,		bool scrollBarVertical = true, bool scrollBarHorizontal = false) = 0;

	//! Adds a mesh viewer. Not 100% implemented yet.
	virtual IGUIMeshViewer* addMeshViewer(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds a file open dialog.
	virtual IGUIFileOpenDialog* addFileOpenDialog(const wchar_t* title=0,		bool modal=true, IGUIElement* parent=0, s32 id=-1, bool restoreCWD=false, io::path::char_type* startDir=0) = 0;

	//! Adds a color select dialog.
	virtual IGUIColorSelectDialog* addColorSelectDialog(const wchar_t* title = 0, bool modal=true, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a static text.
	virtual IGUIStaticText* addStaticText(const wchar_t* text, const core::rect<s32>& rectangle,	bool border=false, bool wordWrap=true, IGUIElement* parent=0, s32 id=-1, bool fillBackground = false) = 0;

	//! Adds an edit box.
	virtual IGUIEditBox* addEditBox(const wchar_t* text, const core::rect<s32>& rectangle, bool border=true, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a spin box.
	virtual IGUISpinBox* addSpinBox(const wchar_t* text, const core::rect<s32>& rectangle, bool border=true,IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds an element for fading in or out.
	virtual IGUIInOutFader* addInOutFader(const core::rect<s32>* rectangle=0, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a tab control to the environment.
	virtual IGUITabControl* addTabControl(const core::rect<s32>& rectangle, IGUIElement* parent=0, bool fillbackground=false,		bool border=true, s32 id=-1) = 0;

	//! Adds tab to the environment.
	virtual IGUITab* addTab(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a context menu to the environment.
	virtual IGUIContextMenu* addContextMenu(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a menu to the environment.
	virtual IGUIContextMenu* addMenu(IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a toolbar to the environment.
	virtual IGUIToolBar* addToolBar(IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a combo box to the environment.
	virtual IGUIComboBox* addComboBox(const core::rect<s32>& rectangle,		IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a table to the environment
	virtual IGUITable* addTable(const core::rect<s32>& rectangle,		IGUIElement* parent=0, s32 id=-1, bool drawBackground=false) =0;

    //! Returns the root gui element.
	virtual IGUIElement* getRootGUIElement() = 0;
    
	//! Get the default element factory which can create all built-in elements
	virtual IGUIElementFactory* getDefaultGUIElementFactory() const = 0;

	//! Adds an element factory to the gui environment.
	virtual void registerGUIElementFactory(IGUIElementFactory* factoryToAdd) = 0;

	//! Get amount of registered gui element factories.
	virtual u32 getRegisteredGUIElementFactoryCount() const = 0;

	//! Get a gui element factory by index
	virtual IGUIElementFactory* getGUIElementFactory(u32 index) const = 0;

	//! Adds a GUI element by its name
	virtual IGUIElement* addGUIElement(const c8* elementName, IGUIElement* parent=0) = 0;

	//! Saves the current gui into a file.
	virtual bool saveGUI(const io::path& filename, IGUIElement* start=0) = 0;

	//! Saves the current gui into a file.
	virtual bool saveGUI(io::IWriteFile* file, IGUIElement* start=0) = 0;

	//! Loads the gui. Note that the current gui is not cleared before.
	virtual bool loadGUI(const io::path& filename, IGUIElement* parent=0) = 0;

	//! Loads the gui. Note that the current gui is not cleared before.
	virtual bool loadGUI(io::IReadFile* file, IGUIElement* parent=0) = 0;

	//! Writes attributes of the gui environment
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const =0;

	//! Reads attributes of the gui environment
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)=0;

	//! writes an element
	virtual void writeGUIElement(io::IXMLWriter* writer, IGUIElement* node) =0;

	//! reads an element
	virtual void readGUIElement(io::IXMLReader* reader, IGUIElement* node) =0;

	virtual void OnUnLoad() = 0;//SETH
	virtual void OnReLoad() = 0;
};


} // end namespace gui
} // end namespace irr

#endif

