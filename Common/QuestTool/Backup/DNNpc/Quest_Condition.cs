using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc
{
    public partial class Quest_Condition : UserControl
    {
        int Condition_Count = 0; // 조건문 카운트 변수
        int Execution_Count = 0; // 실행문 카운트 변수

        // 생성자
        public Quest_Condition()
        {
            InitializeComponent();
        }

        // 컨트롤 로드 이벤트
        private void Quest_Condition_Load(object sender, EventArgs e)
        {
            comboBox_Condition.SelectedIndex = 0;
            comboBox_Execution.SelectedIndex = 0;
        }

        private void Quest_Condition_SizeChanged(object sender, EventArgs e)
        {
            for (int i = 0; i < Parent.Controls.Count; ++i)
            {
                if (Parent.Controls[i].Name != this.Name && Parent.Controls[i].Location.Y > this.Location.Y && i > 0)
                {
                    Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i - 1].Location.Y + Parent.Controls[i - 1].Height + 3);
                }
            }
        }
        
        // 조건문 추가 버튼 클릭 이벤트
        private void button_ConditionAdd_Click(object sender, EventArgs e)
        {
            switch (comboBox_Condition.SelectedIndex)
            {
                case 0: // 레벨체크
                    Conditions.Condition_Level cl = new Conditions.Condition_Level();
                    cl.Location = new Point(10, (31 * Condition_Count) + 40);
                    cl.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cl);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 1: //일반 아이템 체크
                    Conditions.Condition_GeneralItem cg = new Conditions.Condition_GeneralItem();
                    cg.Location = new Point(10, (31 * Condition_Count) + 40);
                    cg.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cg);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 2: // 심볼 아이템 체크
                    Conditions.Condition_SymbolItem cs = new Conditions.Condition_SymbolItem();
                    cs.Location = new Point(10, (31 * Condition_Count) + 40);
                    cs.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cs);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 3: // 클래스 체크
                    Conditions.Condition_Class cc = new Conditions.Condition_Class();
                    cc.Location = new Point(10, (31 * Condition_Count) + 40);
                    cc.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cc);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 4: // 확률 체크
                    Conditions.Condition_Prob cp = new Conditions.Condition_Prob();
                    cp.Location = new Point(10, (31 * Condition_Count) + 40);
                    cp.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cp);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 5: //퀘스트 체크
                    Conditions.Condition_Quest cq = new Conditions.Condition_Quest();
                    cq.Location = new Point(10, (31 * Condition_Count) + 40);
                    cq.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cq);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 6: // 퀘스트 완료 체크
                    Conditions.Condition_Compelete ccom = new Conditions.Condition_Compelete();
                    ccom.Location = new Point(10, (31 * Condition_Count) + 40);
                    ccom.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(ccom);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 7: // 인벤 빈공간 체크
                    Conditions.Condition_Inven ci = new Conditions.Condition_Inven();
                    ci.Location = new Point(10, (31 * Condition_Count) + 40);
                    ci.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(ci);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 8: // 커스터 코드
                    Conditions.Condition_Custum cu = new Conditions.Condition_Custum();
                    cu.Location = new Point(10, (31 * Condition_Count) + 40);
                    cu.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cu);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break; 
            }
        }

        // 조건문 삭제 버튼 클릭 이벤트
        private void button_CondtionDel_Click(object sender, EventArgs e)
        {
            if (Condition_Count > 0)
            {
                groupBox_Condition.Controls[groupBox_Condition.Controls.Count - 1].Dispose();
                --Condition_Count;
                Resize_Condition_Del();
            }
        }

        // 조건문 추가시 컨트롤 리사이즈 함수
        private void Resize_Condition_Add()
        {
            this.Height = this.Height + 31;
            groupBox_Condition.Size = new Size(groupBox_Condition.Width, groupBox_Condition.Height + 31);
            groupBox_Execution.Location = new Point(groupBox_Execution.Location.X, groupBox_Execution.Location.Y + 31);            
        }

        // 조건문 삭제시 컨트롤 리사이즈 함수
        private void Resize_Condition_Del()
        {
            this.Height = this.Height - 31;
            groupBox_Condition.Size = new Size(groupBox_Condition.Width, groupBox_Condition.Height - 31);
            groupBox_Execution.Location = new Point(groupBox_Execution.Location.X, groupBox_Execution.Location.Y - 31);            
        }

        // 실행문 추가시 컨트롤 리사이즈 함수
        private void Resize_Execution_Add()
        {
            this.Height = this.Height + 31;
            groupBox_Execution.Size = new Size(groupBox_Execution.Width, groupBox_Execution.Height + 31);
        }

        // 실행문 삭제시 컨트롤 리사이즈 함수
        private void Resize_Execution_Del()
        {
            this.Height = this.Height - 31;
            groupBox_Execution.Size = new Size(groupBox_Execution.Width, groupBox_Execution.Height - 31);            
        }

        // 실행문 추가 버튼 클릭 이벤트
        private void button_ExeAdd_Click(object sender, EventArgs e)
        {
            switch (comboBox_Execution.SelectedIndex)
            {
                case 0: // 퀘스트 실행
                    Executions.Exe_Quest exe_quest = new Executions.Exe_Quest();
                    exe_quest.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_quest.label_No.Text = Execution_Count.ToString();
                    exe_quest.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_quest);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 1: // 스크립트 실행
                    Executions.Exe_Script exe_script = new Executions.Exe_Script();
                    exe_script.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_script.label_No.Text = Execution_Count.ToString();
                    exe_script.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_script);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 2: // 메세지 출력
                    Executions.Exe_Message exe_message = new Executions.Exe_Message();
                    exe_message.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_message.label_No.Text = Execution_Count.ToString();
                    exe_message.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_message);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 3: // 게임무비 출력
                    Executions.Exe_Movie exe_movie = new Executions.Exe_Movie();
                    exe_movie.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_movie.label_No.Text = Execution_Count.ToString();
                    exe_movie.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_movie);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 4: // 조건문 실행
                    Executions.Exe_Condition exe_condition = new Executions.Exe_Condition();
                    exe_condition.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_condition.label_No.Text = Execution_Count.ToString();
                    exe_condition.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_condition);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 5: // 상점 실행
                    Executions.Exe_Store exe_store = new Executions.Exe_Store();
                    exe_store.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_store.label_No.Text = Execution_Count.ToString();
                    exe_store.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_store);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 6: // 스킬 상점 실행
                    Executions.Exe_SkillStore exe_skill = new Executions.Exe_SkillStore();
                    exe_skill.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_skill.label_No.Text = Execution_Count.ToString();
                    exe_skill.Tag = Execution_Count;
                    groupBox_Execution.Controls.Add(exe_skill);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case 7: // 무인 상점 실행
                    {
                        Executions.Exe_NoManStore exe_noman = new Executions.Exe_NoManStore();
                        exe_noman.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_noman.label_No.Text = Execution_Count.ToString();
                        exe_noman.Tag = Execution_Count;
                        groupBox_Execution.Controls.Add(exe_noman);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        break;
                    }
                case 8: // 문자열 조합 실행
                    {
                        Executions.Exe_StringCombi exe_combi = new Executions.Exe_StringCombi();
                        exe_combi.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_combi.label_No.Text = Execution_Count.ToString();
                        exe_combi.Tag = Execution_Count;
                        groupBox_Execution.Controls.Add(exe_combi);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        break;
                    }
                case 9: // 보옥추가
                    {
                        Executions.Exe_BoOkAdd exe_book = new Executions.Exe_BoOkAdd();
                        exe_book.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_book.label_No.Text = Execution_Count.ToString();
                        exe_book.Tag = Execution_Count;
                        groupBox_Execution.Controls.Add(exe_book);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        break;
                    }
                case 10: // 커스텀 코드
                    {
                        Executions.Exe_Custum exe_custum = new Executions.Exe_Custum();
                        exe_custum.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_custum.label_No.Text = Execution_Count.ToString();
                        exe_custum.Tag = Execution_Count;
                        groupBox_Execution.Controls.Add(exe_custum);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        break;
                    }
                case 11: //컷씬 완료 후, 맵이동.
                    {
                        Executions.Exe_CutsceneCompleteChangeMap executions = new Executions.Exe_CutsceneCompleteChangeMap();
                        executions.Location = new Point(10, (31 * Execution_Count) + 40);
                        executions.label_No.Text = Execution_Count.ToString();
                        executions.Tag = Execution_Count;
                        groupBox_Execution.Controls.Add(executions);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        break;
                    }
            }
        }

        // 실행문 삭제 버튼 클릭 이벤트
        private void button_ExeDel_Click(object sender, EventArgs e)
        {
            if (Execution_Count > 0)
            {
                groupBox_Execution.Controls[groupBox_Execution.Controls.Count - 1].Dispose();
                --Execution_Count;
                Resize_Execution_Del();
            }
        }        

        // 조건문 추가 함수
        public void Add_Condition(Control p_condition)
        {
            p_condition.Location = new Point(10, (31 * Condition_Count) + 40);
            p_condition.Controls["label_No"].Text = Condition_Count.ToString();
            groupBox_Condition.Controls.Add(p_condition);
            Resize_Condition_Add();
            ++Condition_Count;
        }

        // 실행문 추가 함수
        public void Add_Execution(Control p_execution)
        {
            p_execution.Location = new Point(10, (31 * Execution_Count) + 40);
            p_execution.Controls["label_No"].Text = Execution_Count.ToString();
            p_execution.Tag = Execution_Count;
            groupBox_Execution.Controls.Add(p_execution);
            Resize_Execution_Add();
            ++Execution_Count;
        }

        // 조건문 컬렉션 리턴 함수
        public ControlCollection Get_Conditions()
        {
            return groupBox_Condition.Controls;
        }

        // 실행문 컬렉션 리턴 함수
        public ControlCollection Get_Executions()
        {
            return groupBox_Execution.Controls;
        }
    }
}
