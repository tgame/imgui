#pragma once
#include <string>
#include <map>
#include <vector>

namespace imgui_dockable {
    struct LionExtEditorContext
    {

    };

    class DockableUserGUI;
    class DockableSpaceGUI;

    class DockableNode
    {
    public:
        enum DockPos
        {
            center,
            floating,
            left,
            right,
            bottom,
        };
        DockableNode(DockableNode* parent,DockPos dockPos);

        void AddGUIWindow(DockableUserGUI* InGUIWindow, DockableNode::DockPos InDockPos = DockableNode::floating);
        void AddTemplateGUIWindow(const std::string& InGUIWindowName);
        void RenderAllDockSpace(float ViewWidth = -1, float ViewHeight = -1);
        void RegisterTemplateGUIWindow(DockableUserGUI* InGUIWindowTemplate);
        void AddStandardDockSpace();
        float GetDesignedSize();
        void SetDesignedSize(float size);
        int GetDockIndex(DockableSpaceGUI* gui);
        DockableSpaceGUI* InsertOneDockSpace(DockPos dockPos, int index);
        void SetFixedWidth(float v);
        void SetFixedHeight(float v);
        float GetDesignWidth();
        float GetDesignHeight();
        float GetSpliteNearSpaceSize(DockableSpaceGUI* mySpace, bool split_vertically);
        void OnChangeWidth(float width, DockableSpaceGUI* space);
        void OnChangeHeight(float width, DockableSpaceGUI* space);
    protected:
        std::vector<DockableSpaceGUI*>* FindSpaceList(DockableSpaceGUI* space,int& outIndex);
        void GetViewSize(float& offsetx, float& offsety, float& endx, float& endy);
        void CalcDockPosAndSize(float pos, float& finalPosX, float& finalPosY, float& finalSizeX, float& finalSizeY, bool expandFull);
        DockPos GetSelfDockPos() const;
        float GetFixedWidth()const;
        float GetFixedHeight()const;
        void SetView(float offsetx, float offsety, float endx, float endy);
        bool IsCollapse();

        std::vector<DockableUserGUI*> CurrentWindowList;
        std::vector<DockableUserGUI*> TemplateWindowList;

        std::vector<DockableSpaceGUI*> LeftDockSpaceList;
        std::vector<DockableSpaceGUI*> RightDockSpaceList;
        std::vector<DockableSpaceGUI*> BottomSpaceList;
        std::vector<DockableSpaceGUI*> FloatingSpaceList;

        DockableNode* m_ownerNode=nullptr;
        float m_viewStartX = -1;
        float m_viewStartY = -1;
        float m_viewEndX = -1;
        float m_viewEndY = -1;
        DockPos m_selfDockPos = DockPos::floating;
        float m_fixedWidth = 400.0f;
        float m_fixedHeight = 400.0f;
        bool m_isCollapse = false;
        bool m_isAutoHidded = false;
        bool m_isDocking = false;
        float m_leftWidth = 0;
        float m_bottomHeight = 0;
        float m_rightWidth = 0;
    };


    class DockableSpaceGUI
        :public DockableNode
    {
    public:
        struct ScaleableVec2
        {
            float x = 0;
            float y = 0;
            float scale_x = 0;
            float scale_y = 0;
        };
    protected:
        struct WindowInfo
        {
            std::string WindowLionID;
            DockableUserGUI* LionWindow = nullptr;
        };
        struct WindowDragInfo
        {
            DockableUserGUI* LionWindow = nullptr;
            DockableSpaceGUI* SourceDockSpace = nullptr;
        };
        struct DragingButtonRect
        {
            float x;
            float y;
            float width;
            float height;
        };
        DragingButtonRect m_dragUpRect;
        DragingButtonRect m_dragDownRect;
        DragingButtonRect m_dragSelfRect;
        DragingButtonRect m_dragLeftRect;
        DragingButtonRect m_dragRightRect;

        std::vector<WindowInfo> m_dockingWindowList;
        int m_activeWindowIndex = 0;
        std::string m_selfImGuiID;
        ScaleableVec2 m_size;
        ScaleableVec2 m_pos;
        bool m_expandFull = false;
        void DefaultPosAndSize();
        bool ShowCollapse();
        bool ShowTemplateGUIMenu();
        void ShowDockingRects(float posx, float posy, float sizex, float sizey);
        void ShowDockingButtons();
        void DrawOneDragButton(const char* name,const DragingButtonRect& rect,int dockIndexOffset);
        void ShowSplitters(float posx, float posy, float sizex, float sizey);
    public:
        DockableSpaceGUI(DockableNode* owner,DockPos dockPos);
        void DockWindow(DockableUserGUI* InLionWindow);
        void RemoveDockWindow(DockableUserGUI* InLionWindow);
        void OnGUI(float offset, bool expandFull);
        bool IsEmpty();

        static DockableSpaceGUI* Root()
        {
            static DockableSpaceGUI _instance(nullptr,center);
            return &_instance;
        }
    };

    class DockableUserGUI
    {
    protected:
        std::string m_windowNameID;
        std::string m_windowID;
        std::string m_titleText;
        bool m_isCloseable = true;
        bool m_isShowing = true;
        int m_windowImGuiFlag = 0;//ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize
        friend class DockableNode;
        friend class DockableSpaceGUI;
        DockableUserGUI(const std::string& windowNameID, const std::string& titleText);
    public:
        void SetTitle(const std::string& InTitleText);
        const std::string& GetTitle()const;
        void GenerateID();
        virtual DockableUserGUI* DumpCreateWindow();
        virtual void OnGUI() = 0;
    };

    inline const std::string& DockableUserGUI::GetTitle() const
    {
        return m_titleText;
    }


}

namespace ImGui
{
    void RenderTinyDockable();
    void RegisterTinyDockable(imgui_dockable::DockableUserGUI* InTemplateGUI);
    void AddTinyDockableUserGUI(imgui_dockable::DockableUserGUI* InGUI, imgui_dockable::DockableNode::DockPos InDockPos);
}
