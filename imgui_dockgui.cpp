#include "imgui_dockgui.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace imgui_dockable
{
    DockableSpaceGUI::DockableSpaceGUI(DockableNode* owner,DockPos dockPos)
        :DockableNode(owner,dockPos)
    {
        static int id = 0;
        id++;
        char idbuf[255];
        sprintf(idbuf, "DockableSpaceGUI_%d", id);
        m_selfImGuiID = idbuf;
        DefaultPosAndSize();
        if (dockPos == center)
        {
            AddStandardDockSpace();
        }
    }
    bool DockableSpaceGUI_OneSplitter(const char* name, bool split_vertically, const ImRect& bb, float* size1, float* size2)
    {
        using namespace ImGui;
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID(name);
        return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, 50, 50, 0.0f);
    }

    void DockableSpaceGUI::ShowSplitters(float posx, float posy, float sizex, float sizey)
    {
        if (m_isCollapse)
        {
            return;
        }
        float splitterSize = 4;
        ImRect bb;
        bool isChanged = false;
        if (m_selfDockPos == left)
        {
            float otherPanelSize = m_ownerNode->GetSpliteNearSpaceSize(this, true);
            float selfPanelSize = m_fixedWidth;
            bb.Min = ImVec2(posx+sizex- splitterSize*2, posy);
            bb.Max = ImVec2(posx+sizex, posy + sizey);
            isChanged = DockableSpaceGUI_OneSplitter("vert##Splitter", true, bb, &selfPanelSize, &otherPanelSize);
            if (isChanged)
            {
                m_ownerNode->OnChangeWidth(selfPanelSize, this);
            }
            if (!m_expandFull)
            {//show bottom splitter
                otherPanelSize = m_ownerNode->GetSpliteNearSpaceSize(this, false);
                selfPanelSize = m_fixedHeight;
                bb.Min = ImVec2(posx, posy + sizey - splitterSize);
                bb.Max = ImVec2(posx + sizex, posy + sizey);
                isChanged = DockableSpaceGUI_OneSplitter("hor##Splitter", false, bb, &selfPanelSize, &otherPanelSize);
                if (isChanged)
                {
                    m_ownerNode->OnChangeHeight(selfPanelSize, this);
                }
            }
        }
        else if (m_selfDockPos == right)
        {
            float otherPanelSize = m_ownerNode->GetSpliteNearSpaceSize(this, true);
            float selfPanelSize = m_fixedWidth;
            bb.Min = ImVec2(posx - 2, posy);
            bb.Max = ImVec2(posx - 2 + splitterSize * 2, posy + sizey);
            isChanged = DockableSpaceGUI_OneSplitter("vert##Splitter", true, bb, &otherPanelSize, &selfPanelSize);
            if (isChanged)
            {
                m_ownerNode->OnChangeWidth(selfPanelSize, this);
            }
            if (!m_expandFull)
            {//show bottom splitter
                otherPanelSize = m_ownerNode->GetSpliteNearSpaceSize(this, false);
                selfPanelSize = m_fixedHeight;
                bb.Min = ImVec2(posx, posy + sizey - splitterSize);
                bb.Max = ImVec2(posx + sizex, posy + sizey);
                isChanged = DockableSpaceGUI_OneSplitter("hor##Splitter", false, bb, &selfPanelSize, &otherPanelSize);
                if (isChanged)
                {
                    m_ownerNode->OnChangeHeight(selfPanelSize, this);
                }
            }
        }
        else if (m_selfDockPos == bottom)
        {
            float otherPanelSize = m_ownerNode->GetSpliteNearSpaceSize(this, true);
            float selfPanelSize = m_fixedWidth;
            if (!m_expandFull)
            {
                bb.Min = ImVec2(posx + sizex - splitterSize * 2, posy);
                bb.Max = ImVec2(posx + sizex, posy + sizey);
                isChanged = DockableSpaceGUI_OneSplitter("vert##Splitter", true, bb, &selfPanelSize, &otherPanelSize);
                if (isChanged)
                {
                    m_ownerNode->OnChangeWidth(selfPanelSize, this);
                }
            }
            //
            {//show bottom splitter
                otherPanelSize = m_ownerNode->GetSpliteNearSpaceSize(this, false);
                selfPanelSize = m_fixedHeight;
                bb.Min = ImVec2(posx, posy);
                bb.Max = ImVec2(posx + sizex, posy + splitterSize);
                isChanged = DockableSpaceGUI_OneSplitter("hor##Splitter", false, bb, &otherPanelSize, &selfPanelSize);
                if (isChanged)
                {
                    m_ownerNode->OnChangeHeight(selfPanelSize, this);
                }
            }
        }
        
    }

    void DockableSpaceGUI::DefaultPosAndSize()
    {
        if (m_selfDockPos == left)
        {
            m_size.x = 400;
            m_size.scale_y = 1.0f;
            m_size.y = 0;
            m_pos.x = 0;
            m_pos.y = 0;
        }
        if (m_selfDockPos == right)
        {

        }
        if (m_selfDockPos == floating)
        {
            m_size.x = 400;
            m_size.y = 400;
            m_pos.x = 400;
            m_pos.y = 400;
        }
        if (m_selfDockPos == bottom)
        {
            m_size.x = -400;
            m_size.scale_x = 1;
            m_size.y = 400;
            m_size.scale_y = 0;
            m_pos.x = 400;
            m_pos.y = -m_size.y;
            m_pos.scale_y = 1.0f;
        }
    }

    ImVec2 ScaleableVec2_ImVec2(const DockableSpaceGUI::ScaleableVec2& vec2)
    {
        ImGuiIO& io = ImGui::GetIO();
        //ImGuiContext* imguiContext = ImGui::GetCurrentContext();
        float x = io.DisplaySize.x * vec2.scale_x + vec2.x;
        float y = io.DisplaySize.y * vec2.scale_y + vec2.y;
        return ImVec2(x, y);
    }
    void DockableSpaceGUI::DockWindow(DockableUserGUI* InLionWindow)
    {
        WindowInfo info;
        info.LionWindow = InLionWindow;
        m_dockingWindowList.push_back(info);
    }

    void DockableSpaceGUI::RemoveDockWindow(DockableUserGUI* InLionWindow)
    {
        for (int i = 0; i < m_dockingWindowList.size(); i++)
        {
            if (m_dockingWindowList[i].LionWindow == InLionWindow)
            {
                m_dockingWindowList.erase(m_dockingWindowList.begin()+i);
                return;
            }
        }
    }

    bool DockableSpaceGUI::ShowTemplateGUIMenu()
    {
        if (ImGui::Button("+"))
        {
            ImGui::OpenPopup("DockableSpaceGUI_AddTemplateGUIMenu");
        }

        if (ImGui::BeginPopupContextItem("DockableSpaceGUI_AddTemplateGUIMenu"))
        {
            auto& templateGUIList = DockableSpaceGUI::Root()->TemplateWindowList;
            for (auto& gui : templateGUIList)
            {
                if (ImGui::Selectable(gui->GetTitle().c_str()))
                {
                    auto* newWindow = gui->DumpCreateWindow();
                    if (newWindow)
                    {
                        newWindow->GenerateID();
                        DockWindow(newWindow);
                    }
                }
            }
            ImGui::EndPopup();
        }
        return true;
    }
    bool DockableSpaceGUI::ShowCollapse()
    {
        if (m_selfDockPos == center)
        {
            m_isCollapse = false;
            return false;
        }
        if (m_isCollapse)
        {
            auto btnIcon = "+";
            if (ImGui::Button(btnIcon))
            {
                m_isCollapse = false;
            }
        }
        else
        {
            auto btnIcon = "-";
            if (ImGui::Button(btnIcon))
            {
                m_isCollapse = true;
            }
        }
        return true;
    }
    void DockableSpaceGUI::ShowDockingRects(float posx, float posy, float sizex, float sizey)
    {
        if (!m_isDocking)
        {
            return;
        }
        ImGui::SetCursorScreenPos(ImVec2(posx,posy));
        ImGui::BeginChild("ShowDockingRects");
        float barSize = 40;
#if 0
        float pad = 15;
        float minSize = 30;
        if (m_selfDockPos == left || m_selfDockPos == right)
        {
            m_dragUpRect.x = posx + pad;
            m_dragUpRect.width = sizex - pad * 2;
            m_dragUpRect.y = posy + pad;
            m_dragUpRect.height = barSize;

            m_dragSelfRect.x = posx + pad;
            m_dragSelfRect.width = sizex - pad * 2;
            m_dragSelfRect.y = posy + pad + barSize + pad;
            m_dragSelfRect.height = sizey - 4*pad-2*barSize;

            m_dragDownRect.x = posx + pad;
            m_dragDownRect.y = m_dragSelfRect.y + m_dragSelfRect.height + pad;
            m_dragDownRect.width = sizex - pad * 2;
            m_dragDownRect.height = barSize;
        }
        if (m_selfDockPos == bottom)
        {
            m_dragLeftRect.x = posx + pad;
            m_dragLeftRect.y = posy + pad;
            m_dragLeftRect.width = barSize;
            m_dragLeftRect.height = sizey - 2 * pad;

            m_dragSelfRect.x = m_dragLeftRect.x + m_dragLeftRect.width + pad;
            m_dragSelfRect.y = posy + pad;
            m_dragSelfRect.width = sizex-4*pad-2*barSize;
            m_dragSelfRect.height = sizey - 2 * pad;

            m_dragRightRect.x = m_dragSelfRect.x + m_dragSelfRect.width + pad;
            m_dragRightRect.y = posy + pad;
            m_dragRightRect.width = barSize;
            m_dragRightRect.height = sizey - 2 * pad;
        }
        if (m_selfDockPos == center)
        {
            m_dragLeftRect.x = posx + pad;
            m_dragLeftRect.y = posy + pad;
            m_dragLeftRect.width = barSize;
            m_dragLeftRect.height = sizey - 2 * pad;

            m_dragSelfRect.x = m_dragLeftRect.x + m_dragLeftRect.width + pad;
            m_dragSelfRect.y = posy + pad;
            m_dragSelfRect.width = sizex - 4 * pad - 2 * barSize;
            m_dragSelfRect.height = sizey - 2 * pad;

            m_dragRightRect.x = m_dragSelfRect.x + m_dragSelfRect.width + pad;
            m_dragRightRect.y = posy + pad;
            m_dragRightRect.width = barSize;
            m_dragRightRect.height = sizey - 2 * pad;
        }
        if (m_selfDockPos == floating)
        {
            m_dragSelfRect.x = posx + pad;
            m_dragSelfRect.width = sizex - pad;
            m_dragSelfRect.y = posy;
            m_dragSelfRect.height = sizey - pad;
        }
#else
        float smallButtonSize = 20;
        float smallPad = 8;
        ImVec2 centerPos(posx+sizex*0.5f,posy+sizey*0.5f);
        /*
              up
        left self right
             down
        */
        m_dragSelfRect.x = centerPos.x - barSize*0.5f;
        m_dragSelfRect.y = centerPos.y - barSize*0.5f;
        m_dragSelfRect.width = barSize;
        m_dragSelfRect.height = barSize;

        m_dragUpRect.x = centerPos.x - barSize *0.5f;
        m_dragUpRect.y = m_dragSelfRect.y - smallButtonSize - smallPad;
        m_dragUpRect.width = barSize;
        m_dragUpRect.height = smallButtonSize;

        m_dragDownRect.x = centerPos.x - barSize *0.5f;
        m_dragDownRect.y = m_dragSelfRect.y + m_dragSelfRect.height + smallPad;
        m_dragDownRect.width = barSize;
        m_dragDownRect.height = smallButtonSize;

        m_dragLeftRect.x = m_dragSelfRect.x - smallButtonSize - smallPad;
        m_dragLeftRect.y = centerPos.y - barSize * 0.5f;
        m_dragLeftRect.width = smallButtonSize;
        m_dragLeftRect.height = barSize;

        m_dragRightRect.x = m_dragSelfRect.x + m_dragSelfRect.width + smallPad;
        m_dragRightRect.y = centerPos.y - barSize * 0.5f;
        m_dragRightRect.width = smallButtonSize;
        m_dragRightRect.height = barSize;
#endif
        auto* drawList = ImGui::GetCurrentWindow()->DrawList;
        ImU32 color = IM_COL32(128, 128, 128, 255);
        if (m_selfDockPos == left || m_selfDockPos == right)
        {
            drawList->AddRectFilled(ImVec2(m_dragUpRect.x, m_dragUpRect.y), ImVec2(m_dragUpRect.x+ m_dragUpRect.width, m_dragUpRect.y+m_dragUpRect.height), color);
            drawList->AddRectFilled(ImVec2(m_dragSelfRect.x, m_dragSelfRect.y), ImVec2(m_dragSelfRect.x + m_dragSelfRect.width, m_dragSelfRect.y+m_dragSelfRect.height), color);
            drawList->AddRectFilled(ImVec2(m_dragDownRect.x, m_dragDownRect.y), ImVec2(m_dragDownRect.x + m_dragDownRect.width, m_dragDownRect.y+m_dragDownRect.height), color);
        }
        if (m_selfDockPos == bottom)
        {
            drawList->AddRectFilled(ImVec2(m_dragLeftRect.x, m_dragLeftRect.y), ImVec2(m_dragLeftRect.x + m_dragLeftRect.width, m_dragLeftRect.y+m_dragLeftRect.height), color);
            drawList->AddRectFilled(ImVec2(m_dragSelfRect.x, m_dragSelfRect.y), ImVec2(m_dragSelfRect.x + m_dragSelfRect.width, m_dragSelfRect.y+m_dragSelfRect.height), color);
            drawList->AddRectFilled(ImVec2(m_dragRightRect.x, m_dragRightRect.y), ImVec2(m_dragRightRect.x + m_dragRightRect.width, m_dragRightRect.y+m_dragRightRect.height), color);
        }
        if (m_selfDockPos == center || m_selfDockPos == floating)
        {
            drawList->AddRectFilled(ImVec2(m_dragSelfRect.x, m_dragSelfRect.y), ImVec2(m_dragSelfRect.x + m_dragSelfRect.width, m_dragSelfRect.y + m_dragSelfRect.height), color);
        }
        ImGui::EndChild();
    }
    void DockableSpaceGUI::DrawOneDragButton(const char* name, const DragingButtonRect& rect, int dockIndexOffset)
    {
        ImVec2 pos = ImVec2(rect.x, rect.y);
        ImGui::SetCursorScreenPos(pos);
        ImGui::InvisibleButton(name, ImVec2(rect.width, rect.height), ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DockableSpaceGUI"))
            {
                WindowDragInfo dragInfo = *(WindowDragInfo*)payload->Data;
                DockableSpaceGUI* targetSpace = m_ownerNode->InsertOneDockSpace(m_selfDockPos, m_ownerNode->GetDockIndex(this)+ dockIndexOffset);
                if (targetSpace)
                {
                    dragInfo.SourceDockSpace->RemoveDockWindow(dragInfo.LionWindow);
                    targetSpace->DockWindow(dragInfo.LionWindow);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    void DockableSpaceGUI::ShowDockingButtons()
    {
        ImVec2 pos(m_dragSelfRect.x, m_dragSelfRect.y);
        ImGui::SetCursorScreenPos(pos);
        ImGui::InvisibleButton("Drag into dock space", ImVec2(m_dragSelfRect.width,m_dragSelfRect.height), ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DockableSpaceGUI"))
            {
                WindowDragInfo dragInfo = *(WindowDragInfo*)payload->Data;
                dragInfo.SourceDockSpace->RemoveDockWindow(dragInfo.LionWindow);
                DockWindow(dragInfo.LionWindow);
            }
            ImGui::EndDragDropTarget();
        }
        if (m_selfDockPos == left || m_selfDockPos == right)
        {
            DrawOneDragButton("Drag into up dock space", m_dragUpRect, 0);
            DrawOneDragButton("Drag into down dock space", m_dragDownRect, 1);
        }
        if (m_selfDockPos == bottom)
        {
            DrawOneDragButton("Drag into left dock space", m_dragLeftRect, 0);
            DrawOneDragButton("Drag into right dock space", m_dragRightRect, 1);
        }
    }
    bool DockableSpaceGUI::IsEmpty()
    {
        return m_dockingWindowList.size() == 0;
    }
    void DockableSpaceGUI::OnGUI(float offset, bool expandFull)
    {
        m_expandFull = expandFull;
        ImGuiContext* imguiContext = ImGui::GetCurrentContext();

        bool isSelfShow = m_dockingWindowList.size() > 0;
        //isSelfShow=true;
        if (m_selfDockPos == center)
        {
            isSelfShow = true;
        }
        if (m_selfDockPos == floating)
        {
            isSelfShow = true;
        }
        m_isDocking = false;
        if (imguiContext->DragDropActive)
        {
            if (strcmp(imguiContext->DragDropPayload.DataType, "DockableSpaceGUI") == 0)
            {
                m_isDocking = true;
                isSelfShow = true;
            }
        }
        m_isAutoHidded = !isSelfShow;
        if (!isSelfShow)
        {
            return;
        }
        int selfWindowFlag = 0;
        if (m_selfDockPos != floating)
        {
            selfWindowFlag = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus
                | ImGuiWindowFlags_NoNav |ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;// | ImGuiWindowFlags_NoMove;        
        }
        ImVec2 pos = ScaleableVec2_ImVec2(m_pos);
        ImVec2 size = ScaleableVec2_ImVec2(m_size);
        CalcDockPosAndSize(offset, pos.x, pos.y, size.x, size.y, expandFull);
        if (m_selfDockPos != floating)
        {
            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(size);
        }
        else
        {
            ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        }
        ImGui::Begin(m_selfImGuiID.c_str(), &isSelfShow, selfWindowFlag);
        //ImGui::Text("width:%3.0f,height:%3.0f", size.x, size.y);
        ShowSplitters(pos.x, pos.y, size.x, size.y);
        

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        bool needSameLine = false;
        //if (!m_isDocking)
        {
            needSameLine = ShowCollapse();
            if (!m_isCollapse)
            {
                if (needSameLine)
                {
                    ImGui::SameLine();
                }
                needSameLine |= ShowTemplateGUIMenu();
            }
        }
        char idbuf[1024];

        if (!m_isCollapse)
        {
            if (needSameLine)
            {
                ImGui::SameLine();
            }
            if (ImGui::BeginTabBar("MainTabBar", tab_bar_flags))
            {
                for (int i = 0; i < m_dockingWindowList.size(); i++)
                {
                    WindowInfo& info = m_dockingWindowList[i];
                    //if(ImGui::Button(info.LionWindow->GetTitle()))
                    //ImGuiTabItemFlags_Leading
                    auto* window = info.LionWindow;
                    sprintf(idbuf, "%s##%s", window->m_titleText.c_str(), window->m_windowID.c_str());
                    if (ImGui::BeginTabItem(idbuf))
                    {
                        m_activeWindowIndex = i;
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        // Set payload to carry the index of our item (could be anything)
                        WindowDragInfo dragInfo;
                        dragInfo.LionWindow = info.LionWindow;
                        dragInfo.SourceDockSpace = this;
                        ImGui::SetDragDropPayload("DockableSpaceGUI", &dragInfo, sizeof(dragInfo));
                        ImGui::Text("Drag window to other dock space");
                        ImGui::EndDragDropSource();
                    }
                }
                ImGui::EndTabBar();
            }
        }
        if (!m_isCollapse && m_activeWindowIndex >= 0 && m_activeWindowIndex < m_dockingWindowList.size())
        {
            std::string titleAndID = idbuf;
            WindowInfo& info = m_dockingWindowList[m_activeWindowIndex];
            auto* window = info.LionWindow;
            sprintf(idbuf, "%s##%s", window->m_titleText.c_str(), window->m_windowID.c_str());
            ImGui::BeginChild(titleAndID.c_str());
            {
                window->OnGUI();
                ImGui::EndChild();
            }
        }

        if (m_isDocking)
        {
            ShowDockingRects(pos.x, pos.y, size.x, size.y);
            ShowDockingButtons();
        }
        ImGui::End();
        RenderAllDockSpace();
    }



    DockableUserGUI::DockableUserGUI(const std::string& windowNameID, const std::string& titleText)
    {
        m_windowNameID = windowNameID;
        m_titleText = titleText;
    }
    void DockableUserGUI::SetTitle(const std::string& InTitleText)
    {
        m_titleText = InTitleText;
    }

    void DockableUserGUI::GenerateID()
    {
        static int id = 9878123;

        char idbuf[255];
        sprintf(idbuf, "GUI_%d", id);
        
        m_windowID = idbuf;
        id++;
    }

    DockableUserGUI* DockableUserGUI::DumpCreateWindow()
    {
        return nullptr;
    }

    DockableNode::DockableNode(DockableNode* parent,DockPos dockPos)
        :m_selfDockPos(dockPos)
        ,m_ownerNode(parent)
    {

    }
    void DockableNode::AddGUIWindow(DockableUserGUI* InGUIWindow, DockableSpaceGUI::DockPos InDockPos)
    {
        if (InGUIWindow->m_windowID.size() == 0)
        {
            InGUIWindow->GenerateID();
        }
        CurrentWindowList.push_back(InGUIWindow);
        if (InDockPos == DockableSpaceGUI::DockPos::floating)
        {
            FloatingSpaceList[0]->DockWindow(InGUIWindow);
        }
        else if (InDockPos == DockableSpaceGUI::DockPos::left)
        {
            LeftDockSpaceList[0]->DockWindow(InGUIWindow);
        }
        else if (InDockPos == DockableSpaceGUI::DockPos::right)
        {
            RightDockSpaceList[0]->DockWindow(InGUIWindow);
        }
        else if (InDockPos == DockableSpaceGUI::DockPos::bottom)
        {
            BottomSpaceList[0]->DockWindow(InGUIWindow);
        }
    }

    void DockableNode::AddTemplateGUIWindow(const std::string& InGUIWindowName)
    {
        for (auto* window : TemplateWindowList)
        {
            if (window->m_windowNameID == InGUIWindowName)
            {
                auto* newWindow = window->DumpCreateWindow();
                if (newWindow)
                {
                    newWindow->GenerateID();
                    AddGUIWindow(newWindow);
                }
                return;
            }
        }
    }

    void DockableNode::RenderAllDockSpace(float ViewWidth, float ViewHeight)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (ViewWidth < 0)
        {
            ViewWidth = io.DisplaySize.x;
        }
        if (ViewHeight < 0)
        {
            ViewHeight = io.DisplaySize.y;
        }
        m_leftWidth = 0;
        m_rightWidth = 0;
        m_bottomHeight = 0;

        for (DockableSpaceGUI* space : LeftDockSpaceList)
        {
            space->SetView(0, 0, ViewWidth, ViewHeight);
            if (space->GetFixedWidth() > m_leftWidth)
            {
                m_leftWidth = space->GetFixedWidth();
            }
        }
        for (DockableSpaceGUI* space : BottomSpaceList)
        {
            space->SetView(m_leftWidth, 0, ViewWidth, ViewHeight);
            if (space->GetFixedHeight() > m_bottomHeight)
            {
                m_bottomHeight = space->GetFixedHeight();
            }
        }
        for (DockableSpaceGUI* space : RightDockSpaceList)
        {
            space->SetView(0, 0, ViewWidth, ViewHeight - m_bottomHeight);
            if (space->GetFixedWidth() > m_rightWidth)
            {
                m_rightWidth = space->GetFixedWidth();
            }
        }
        float posX = 0.0f;
        //left
        float posY = 0.0f;
        for (int i = 0; i < LeftDockSpaceList.size();)
        {
            DockableSpaceGUI* space = LeftDockSpaceList[i];
            if (LeftDockSpaceList.size() > 1)
            {
                if (space->IsEmpty())
                {
                    LeftDockSpaceList.erase(LeftDockSpaceList.begin() + i);
                    delete space;
                    continue;
                }
            }
            space->OnGUI(posY, i == LeftDockSpaceList.size() - 1);
            posY += space->GetFixedHeight();
            i++;
        }
        //Bottom
        posX = 0;
        for (int i = 0; i < BottomSpaceList.size();)
        {
            DockableSpaceGUI* space = BottomSpaceList[i];
            if (BottomSpaceList.size() > 1)
            {
                if (space->IsEmpty())
                {
                    BottomSpaceList.erase(BottomSpaceList.begin() + i);
                    delete space;
                    continue;
                }
            }
            //space->SetFixedHeight(m_bottomHeight);
            space->OnGUI(posX, i == BottomSpaceList.size() - 1);
            posX += space->GetFixedWidth();
            i++;
        }
        //right
        posY = 0.0f;
        for (int i = 0; i < RightDockSpaceList.size(); )
        {
            DockableSpaceGUI* space = RightDockSpaceList[i];
            if (RightDockSpaceList.size() > 1)
            {
                if (space->IsEmpty())
                {
                    RightDockSpaceList.erase(RightDockSpaceList.begin() + i);
                    delete space;
                    continue;
                }
            }
            //space->SetFixedWidth(m_rightWidth);
            space->OnGUI(posY, i == RightDockSpaceList.size() - 1);
            posY += space->GetFixedHeight();
            i++;
        }
        //right
        posY = 0.0f;
        for (int i = 0; i < FloatingSpaceList.size(); )
        {
            DockableSpaceGUI* space = FloatingSpaceList[i];
            if (FloatingSpaceList.size() > 1)
            {
                if (space->IsEmpty())
                {
                    FloatingSpaceList.erase(FloatingSpaceList.begin() + i);
                    delete space;
                    continue;
                }
            }
            space->OnGUI(posY, i == FloatingSpaceList.size() - 1);
            i++;
        }
    }
    float DockableNode::GetSpliteNearSpaceSize(DockableSpaceGUI* space, bool split_vertically)
    {
        float startx;
        float endx;
        float starty;
        float endy;
        GetViewSize(startx, starty, endx, endy);
        float viewWidth = endx - startx;
        float viewHeight = endy - starty;
        DockPos dockPos = space->m_selfDockPos;
        if (dockPos == right)
        {
            if (split_vertically)
            {
                return viewWidth - m_leftWidth;
            }
            else
            {
                for (int i = 0; i < RightDockSpaceList.size(); i++)
                {
                    auto* spaceInList = RightDockSpaceList[i];
                    if (spaceInList == space && (i + 1) < RightDockSpaceList.size())
                    {
                        auto* nextSpace = RightDockSpaceList[i + 1];
                        float nearSpaceSize = nextSpace->GetFixedHeight();
                        return nearSpaceSize;
                    }
                }
            }
        }
        else if (dockPos == left)
        {
            if (split_vertically)
            {
                return viewWidth - m_rightWidth;
            }
            else
            {
                for (int i = 0; i < LeftDockSpaceList.size(); i++)
                {
                    auto* spaceInList = LeftDockSpaceList[i];
                    if (spaceInList == space && (i + 1) < LeftDockSpaceList.size())
                    {
                        auto* nextSpace = LeftDockSpaceList[i + 1];
                        float nearSpaceSize = nextSpace->GetFixedHeight();
                        return nearSpaceSize;
                    }
                }
            }
        }
        else if (dockPos == bottom)
        {
            if (split_vertically)
            {
                for (int i = 0; i < BottomSpaceList.size(); i++)
                {
                    auto* spaceInList = BottomSpaceList[i];
                    if (spaceInList == space && (i + 1) < BottomSpaceList.size())
                    {
                        auto* nextSpace = BottomSpaceList[i + 1];
                        float nearSpaceSize = nextSpace->GetFixedWidth();
                        return nearSpaceSize;
                    }
                }
            }
            else
            {
                return viewHeight - m_bottomHeight;
            }
        }

        return -1.0f;
    }
    void DockableNode::OnChangeWidth(float width, DockableSpaceGUI* space)
    {
        DockPos dockPos = space->m_selfDockPos;
        std::vector<DockableSpaceGUI*>* spaceList = nullptr;
        if (dockPos == left)
        {
            spaceList = &LeftDockSpaceList;
        }
        if (dockPos == right)
        {
            spaceList = &RightDockSpaceList;
        }
        if (dockPos == bottom)
        {
            for (int i = 0; i < BottomSpaceList.size(); i++)
            {
                auto* spaceInList = BottomSpaceList[i];
                if (spaceInList == space && (i + 1) < BottomSpaceList.size())
                {
                    auto* nextSpace = BottomSpaceList[i + 1];
                    float oldValue = space->GetFixedWidth();
                    float heightInc = width - oldValue;
                    space->SetFixedWidth(oldValue + heightInc);
                    nextSpace->SetFixedWidth(nextSpace->GetFixedWidth() - heightInc);
                }
            }
        }
        else if(spaceList) {
            for (auto* spaceInList : *spaceList)
            {
                spaceInList->SetFixedWidth(width);
            }
        }
    }
    void DockableNode::OnChangeHeight(float height, DockableSpaceGUI* space)
    {
        DockPos dockPos = space->m_selfDockPos;
        std::vector<DockableSpaceGUI*>* spaceList = nullptr;
        if (dockPos == left)
        {
            spaceList = &LeftDockSpaceList;
        }
        if (dockPos == right)
        {
            spaceList = &RightDockSpaceList;
        }
        if (dockPos == bottom)
        {
            for (auto* spaceInList : BottomSpaceList)
            {
                spaceInList->SetFixedHeight(height);
            }
        }
        else if (spaceList)
        {
            for (int i = 0; i < (*spaceList).size(); i++)
            {
                auto* spaceInList = (*spaceList)[i];
                if (spaceInList == space && (i + 1) < (*spaceList).size())
                {
                    auto* nextSpace = (*spaceList)[i + 1];
                    float oldHeight = space->GetFixedHeight();
                    float heightInc = height - oldHeight;
                    space->SetFixedHeight(oldHeight + heightInc);
                    nextSpace->SetFixedHeight(nextSpace->GetFixedHeight() - heightInc);
                }
            }
        }
    }

    void DockableNode::GetViewSize(float& offsetx, float& offsety, float& endx, float& endy)
    {
        ImGuiIO& io = ImGui::GetIO();
        offsetx = 0;
        offsety = 0;
        endx = io.DisplaySize.x;
        endy = io.DisplaySize.y;
        if (m_viewStartX >= 0)
        {
            offsetx = m_viewStartX;
        }
        if (m_viewStartY >= 0)
        {
            offsety = m_viewStartY;
        }
        if (m_viewEndX >= 0)
        {
            endx = m_viewEndX;
        }
        if (m_viewEndY >= 0)
        {
            endy = m_viewEndY;
        }
    }
    void DockableNode::CalcDockPosAndSize(float offset, float& finalPosX, float& finalPosY, float& finalSizeX,
        float& finalSizeY, bool expandFull)
    {
        float viewEndX = 400;
        float viewEndY = 400;
        float viewOffsetX = 0;
        float viewOffsetY = 0;
        GetViewSize(viewOffsetX, viewOffsetY, viewEndX, viewEndY);
        if (m_selfDockPos == left)
        {
            if (expandFull)
            {
                m_fixedHeight = viewEndY - offset - viewOffsetY;
            }
            finalPosX = viewOffsetX;
            finalPosY = offset;
            finalSizeX = m_fixedWidth;
            finalSizeY = m_fixedHeight;
            if (m_isCollapse)
            {
                finalSizeX = 30;
            }
        }
        else if (m_selfDockPos == right)
        {
            if (expandFull)
            {
                m_fixedHeight = viewEndY - offset - viewOffsetY;
            }
            finalPosX = viewEndX - m_fixedWidth;
            finalPosY = offset;
            finalSizeX = m_fixedWidth;
            finalSizeY = m_fixedHeight;
            if (m_isCollapse)
            {
                finalSizeX = 30;
                finalPosX = viewEndX - finalSizeX;
            }
        }
        else if (m_selfDockPos == bottom)
        {
            if (expandFull)
            {
                m_fixedWidth = viewEndX - offset - viewOffsetX;
            }
            finalPosX = offset + viewOffsetX;
            finalPosY = viewEndY - m_fixedHeight;
            finalSizeX = m_fixedWidth;
            finalSizeY = m_fixedHeight;
            if (m_isCollapse)
            {
                finalSizeY = 30;
                finalPosY = viewEndY - finalSizeY;
            }
        }
        else if(m_selfDockPos == center)
        {
            finalPosX = viewOffsetX+m_leftWidth;
            finalPosY = viewOffsetY;
            finalSizeX = viewEndX - viewOffsetX-m_rightWidth-m_leftWidth;
            finalSizeY = viewEndY - viewOffsetY-m_bottomHeight;
        }
    }

    DockableNode::DockPos DockableNode::GetSelfDockPos() const
    {
        return m_selfDockPos;
    }
    float DockableNode::GetDesignedSize()
    {
        if (m_selfDockPos == left || m_selfDockPos == right)
        {
            return m_fixedHeight;
        }
        if (m_selfDockPos == bottom)
        {
            return m_fixedWidth;
        }


        return m_fixedWidth;
    }
    void DockableNode::SetDesignedSize(float size)
    {
        if (m_selfDockPos == left || m_selfDockPos == right)
        {
            m_fixedHeight = size;
        }
        if (m_selfDockPos == bottom)
        {
            m_fixedWidth = size;
        }
    }
    float DockableNode::GetDesignWidth()
    {
        return m_fixedWidth;
    }
    float DockableNode::GetDesignHeight()
    {
        return m_fixedHeight;
    }
    float DockableNode::GetFixedWidth() const
    {
        if (m_isCollapse && (m_selfDockPos == left || m_selfDockPos == right))
        {
            return 30;
        }
        if (m_isAutoHidded && (m_selfDockPos == left || m_selfDockPos == right))
        {
            return 0;
        }
        return m_fixedWidth;
    }

    void DockableNode::SetFixedWidth(float v)
    {
        m_fixedWidth = v;
    }

    float DockableNode::GetFixedHeight() const
    {
        if (m_isCollapse && (m_selfDockPos == bottom))
        {
            return 30;
        }
        if (m_isAutoHidded && (m_selfDockPos == bottom))
        {
            return 0;
        }
        return m_fixedHeight;
    }

    std::vector<DockableSpaceGUI*>* DockableNode::FindSpaceList(DockableSpaceGUI* space, int& outIndex)
    {
        DockPos dockPos = space->m_selfDockPos;
        std::vector<DockableSpaceGUI*>* spaceList = nullptr;
        if (dockPos == left)
        {
            spaceList = &LeftDockSpaceList;
        }
        if (dockPos == right)
        {
            spaceList = &RightDockSpaceList;
        }
        if (dockPos == bottom)
        {
            spaceList = &BottomSpaceList;
        }
        if (dockPos == floating)
        {
            spaceList = &FloatingSpaceList;
        }
        if (spaceList)
        {
            for (int i = 0; i < spaceList->size(); i++)
            {
                if ((*spaceList)[i] == space)
                {
                    outIndex = i;
                    return spaceList;
                }
            }
        }
        return nullptr;
    }
    void DockableNode::SetFixedHeight(float v)
    {
        m_fixedHeight = v;
    }

    void DockableNode::SetView(float offsetx, float offsety, float endx, float endy)
    {
        m_viewStartX = offsetx;
        m_viewStartY = offsety;
        m_viewEndX = endx;
        m_viewEndY = endy;
    }

    bool DockableNode::IsCollapse()
    {
        return m_isCollapse;
    }
    void DockableNode::RegisterTemplateGUIWindow(DockableUserGUI* InGUIWindowTemplate)
    {
        TemplateWindowList.push_back(InGUIWindowTemplate);
    }
    int DockableNode::GetDockIndex(DockableSpaceGUI* gui)
    {
        DockPos dockPos = gui->m_selfDockPos;
        std::vector<DockableSpaceGUI*>* spaceList = nullptr;
        if (dockPos == left)
        {
            spaceList = &LeftDockSpaceList;
        }
        if (dockPos == right)
        {
            spaceList = &RightDockSpaceList;
        }
        if (dockPos == bottom)
        {
            spaceList = &BottomSpaceList;
        }
        if (dockPos == floating)
        {
            spaceList = &FloatingSpaceList;
        }
        if (spaceList)
        {
            for (int i = 0; i < spaceList->size(); i++)
            {
                if ((* spaceList)[i] == gui)
                {
                    return i;
                }
            }
        }
        return 0;
    }
    DockableSpaceGUI* DockableNode::InsertOneDockSpace(DockPos dockPos, int index)
    {
        std::vector<DockableSpaceGUI*>* spaceList = nullptr;
        if (dockPos == left)
        {
            spaceList = &LeftDockSpaceList;
        }
        if (dockPos == right)
        {
            spaceList = &RightDockSpaceList;
        }
        if (dockPos == bottom)
        {
            spaceList = &BottomSpaceList;
        }
        if (spaceList == nullptr)
        {
            return nullptr;
        }
        if (spaceList->size() == 1 && (*spaceList)[0]->IsEmpty())
        {
            return (*spaceList)[0];
        }
        int dockIndex = index;
        float designedSize = 400;
        DockableSpaceGUI* nearSpace = nullptr;
        if (dockIndex <= 0)
        {
            dockIndex = 0;
            if (spaceList->size() > 0)
            {
                nearSpace = (*spaceList)[0];
            }
        }
        else if (dockIndex >= spaceList->size())
        {
            dockIndex = spaceList->size();
            if (spaceList->size() > 0)
            {
                nearSpace = (*spaceList)[spaceList->size()-1];
            }
        }
        else {
            nearSpace = (*spaceList)[dockIndex];
        }
        if (nearSpace)
        {
            float nearSpaceSize = nearSpace->GetDesignedSize();
            designedSize = nearSpaceSize * 0.5f;
            nearSpace->SetDesignedSize(designedSize);
        }
        DockableSpaceGUI* space = new DockableSpaceGUI(this, dockPos);
        if (nearSpace)
        {
            space->SetFixedHeight(nearSpace->GetFixedHeight());
            space->SetFixedWidth(nearSpace->GetFixedWidth());
        }
        space->SetDesignedSize(designedSize);
        spaceList->insert(spaceList->begin() + dockIndex, space);

        return space;
    }
    void DockableNode::AddStandardDockSpace()
    {
        DockableSpaceGUI* space = nullptr;
        space = new DockableSpaceGUI(this,DockableSpaceGUI::DockPos::left);
        LeftDockSpaceList.push_back(space);
        space = new DockableSpaceGUI(this,DockableSpaceGUI::DockPos::bottom);
        BottomSpaceList.push_back(space);
        space = new DockableSpaceGUI(this,DockableSpaceGUI::DockPos::right);
        RightDockSpaceList.push_back(space);
        space = new DockableSpaceGUI(this,DockableSpaceGUI::DockPos::floating);
        FloatingSpaceList.push_back(space);
    }

}



namespace ImGui
{
    void RenderTinyDockable()
    {
        imgui_dockable::DockableSpaceGUI::Root()->OnGUI(0,false);
    }
    void RegisterTinyDockable(imgui_dockable::DockableUserGUI* InTemplateGUI)
    {
        imgui_dockable::DockableSpaceGUI::Root()->RegisterTemplateGUIWindow(InTemplateGUI);
    }
    void AddTinyDockableUserGUI(imgui_dockable::DockableUserGUI* InGUI, imgui_dockable::DockableNode::DockPos InDockPos)
    {
        imgui_dockable::DockableSpaceGUI::Root()->AddGUIWindow(InGUI, InDockPos);
    }
}
