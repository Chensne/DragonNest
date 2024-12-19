using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_NPC : UserControl
    {
        public Quest_Unit qu;
        public int Step_Count = 1;
        // 단계 순서 배열(단계시작에서 알파벳 순으로)
        private string[] array_alphabet = { "단계시작","a", "b", "c", "d", "e", "f", "g", "h", "i", 
                                              "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", 
                                              "u", "v", "w", "x", "y", "z" };

        private void Swap(ref Quest_Step x, ref Quest_Step y)
        {
            Quest_Step temp = x;
            x = y;
            y = temp;
        }

        private void Quick_Sort(Quest_Step[] arr, int left, int right)
        {
            if (right > left)
            {
                Quest_Step temp = arr[left];
                int temp2 = 0;
                int j = left;
                for (int i = left + 1; i <= right; i++)
                    if (temp != null && arr[i] != null)
                    {   
                        string index1 = arr[i].Get_StepNo();
                        string index2 = temp.Get_StepNo();

                        if (index1 == "단계시작") 
                            index1 = "";

                        if (index2 == "단계시작") 
                            index2 = "";

                        if (index1.CompareTo(index2) < 0)
                        {
                            j++;
                            Swap(ref arr[i], ref arr[j]);
                        }
                    }
                temp2 = j;
                Swap(ref arr[left], ref arr[temp2]);

                Quick_Sort(arr, left, temp2 - 1);
                Quick_Sort(arr, temp2 + 1, right);
            }
        }

        // 생성자(상위 유닛 파라미터)
        public Quest_NPC(Quest_Unit p_qu)
        {
            InitializeComponent();
            qu = p_qu;           
        }

        // 생성자(상위 유닛 파라미터, 기본 NPC 이름, 기본 실행번호)
        public Quest_NPC(Quest_Unit p_qu, string p_name, string p_no)
        {
            InitializeComponent();

            qu = p_qu;  
            textBox_ExecuteNo.Text = p_no;
            textBox_NPCName.Text = p_name;
        }

        // 값 셋팅(NPC이름, 실행번호)
        public void Set_Value(string p_name, string p_no)
        {
            textBox_ExecuteNo.Text = p_no;
            textBox_NPCName.Text = p_name;
        }

        // 대사 추가(빈 대사 추가)
        private void Add_SubStep()
        {
            Quest_Unit parent = (Quest_Unit)this.Parent;

            Quest_Step qs = new Quest_Step(this);

            qs.Location = new Point(193, groupBox_NPC.Height + 6); //퀘스트 스텝 위치 설정
            qs.Name = "step_" + Step_Count;     // 퀘스트 스텝 이름 설정(step_번호)
            qs.Tag = Step_Count;    // 퀘스트 번호를 테그로 설정
            qs.textBox_StepNo.Text = array_alphabet[Step_Count]; // 퀘스트 단계 번호설정

            // 스텝 추가로 인한 NPC 창 크기 및 그룹박스 크기 변경
            this.Size = new Size(this.Size.Width, this.Size.Height + qs.Height);
            groupBox_NPC.Size = new Size(groupBox_NPC.Size.Width, groupBox_NPC.Size.Height + qs.Height);

            // 스텝 추가로 인한 유닛 창 크기 및 그룹박스 크기 변경
            parent.Size = new Size(parent.Size.Width, parent.Size.Height + qs.Height);
            parent.groupBox_Unit.Height = parent.groupBox_Unit.Height + qs.Height;

            // 현재 NPC 창 이후에 있는 모든 컨트롤의 위치를 추가된 스텝 Height만큼 뒤로 밀기
            for (int i = 0; i < parent.Controls.Count; ++i)
            {
                if (parent.Controls[i].Name.IndexOf("npc_init") < 0 && parent.Controls[i].Name.IndexOf("npc_") >= 0)
                {
                    int controlNo = Convert.ToInt16(parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("npc_init") < 0)
                        thisNo = Convert.ToInt16(this.Tag);

                    if (controlNo > thisNo)
                        parent.Controls[i].Location = new Point(parent.Controls[i].Location.X, parent.Controls[i].Location.Y + qs.Height);
                }
            }

            this.Controls.Add(qs); //생성된 스텝 추가
            qs.textBox_Script.Focus();
            Step_Count++; // 갯수 증가
        }

        // 대사 추가(NPC 로딩시 사용-상세 내용은 위의 대사추가 함수와 동일)
        public void Add_SubStep(Quest_Step p_step)
        {
            p_step.Location = new Point(193, this.Controls[this.Controls.Count - 1].Location.Y + this.Controls[this.Controls.Count - 1].Height);
            p_step.Name = "step_" + Step_Count;
            p_step.Tag = Step_Count;

            this.Size = new Size(this.Size.Width, this.Size.Height + 195);
            groupBox_NPC.Size = new Size(groupBox_NPC.Size.Width, groupBox_NPC.Size.Height + 195);

            qu.Size = new Size(qu.Size.Width, qu.Size.Height + 200 );

            for (int i = 0; i < qu.Controls.Count; ++i)
            {
                if (qu.Controls[i].Name.IndexOf("npc_init") < 0 && qu.Controls[i].Name.IndexOf("npc_") >= 0)
                {
                    int controlNo = Convert.ToInt16(qu.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("npc_init") < 0)
                        thisNo = Convert.ToInt16(this.Tag);

                    if (controlNo > thisNo)
                        qu.Controls[i].Location = new Point(qu.Controls[i].Location.X, qu.Controls[i].Location.Y + 195);
                }
            }

            this.Controls.Add(p_step);
            Step_Count++;
        }

        // 대사 추가(중간단계추가)
        public void Insert_SubStep(string p_name, string p_number)
        {
            Quest_Unit parent = (Quest_Unit)this.Parent;
            Quest_Step qs = new Quest_Step(this);
            Quest_Step prev_step = null;
            // 선택된 NPC 찾기
            for (int i = 0; i < this.Controls.Count; ++i)
            {
                if (this.Controls[i].Name == p_name)
                {
                    prev_step = (Quest_Step)this.Controls[i];
                    break;
                }                
            }
            // 선택된 NPC가 없을 경우 그냥 리턴
            if (prev_step == null)
                return;

            // 현재 NPC 내부 컨트롤 위치 변경
            if (prev_step.Tag == null)
            {
                for (int i = 0; i < this.Controls.Count; ++i)
                {
                    if (this.Controls[i].Tag != null)
                    {
                        this.Controls[i].Location = new Point(this.Controls[i].Location.X, this.Controls[i].Location.Y + 195);

                        int step_no = 0;
                        if (Int32.TryParse(this.Controls[i].Tag.ToString(), out step_no))
                        {
                            Quest_Step temp_step = (Quest_Step)this.Controls[i];
                            temp_step.Tag = step_no + 1;
                            temp_step.Name = "step_" + temp_step.Tag.ToString();
                            temp_step.textBox_StepNo.Text = array_alphabet[step_no + 1];
                        }
                    }
                }                

                qs.Location = new Point(193, prev_step.Location.Y + prev_step.Height + 1); //새로운 스텝 위치 설정
                qs.Name = "step_1"; // 새로운 퀘스트 스텝 이름 설정(step_번호)
                qs.Tag = 1; // 새로운 퀘스트 번호를 테그로 설정
                qs.textBox_StepNo.Text = array_alphabet[1]; // 새로운 퀘스트 단계 번호설정

                this.Controls.Add(qs); //생성된 스텝 추가                

                Step_Count++; // 갯수 증가
            }
            else
            {
                int temp = 0;
                if(Int32.TryParse(p_number,out temp))
                {
                    for (int i = 0; i < this.Controls.Count; ++i)
                    {
                        if (this.Controls[i].Tag != null && temp < Convert.ToInt32(this.Controls[i].Tag))
                        {
                            this.Controls[i].Location = new Point(this.Controls[i].Location.X, this.Controls[i].Location.Y + 195);

                            int step_no = 0;
                            if (Int32.TryParse(this.Controls[i].Tag.ToString(), out step_no))
                            {
                                Quest_Step temp_step = (Quest_Step)this.Controls[i];
                                temp_step.Tag = step_no + 1;
                                temp_step.Name = "step_" + temp_step.Tag.ToString();
                                temp_step.textBox_StepNo.Text = array_alphabet[step_no + 1];
                            }
                        }
                    }
                }               

                qs.Location = new Point(193, prev_step.Location.Y + prev_step.Height + 1); //새로운 스텝 위치 설정
                qs.Name = "step_" + (temp+1).ToString(); // 새로운 퀘스트 스텝 이름 설정(step_번호)
                qs.Tag = temp + 1; // 새로운 퀘스트 번호를 테그로 설정
                qs.textBox_StepNo.Text = array_alphabet[temp+1]; // 새로운 퀘스트 단계 번호설정

                this.Controls.Add(qs); //생성된 스텝 추가                  

                Step_Count++; // 갯수 증가
            }            

            // 스텝 추가로 인한 NPC 창 크기 및 그룹박스 크기 변경
            this.Size = new Size(this.Size.Width, this.Size.Height + 195);
            groupBox_NPC.Size = new Size(groupBox_NPC.Size.Width, groupBox_NPC.Size.Height + 195);

            // 스텝 추가로 인한 유닛 창 크기 및 그룹박스 크기 변경
            parent.Size = new Size(parent.Size.Width, parent.Size.Height + 195);
            parent.groupBox_Unit.Height = parent.groupBox_Unit.Height + 195;

            //하위 NPC 위치 변경
            for (int i = 0; i < parent.Controls.Count; ++i)
            {
                if (parent.Controls[i].Name.IndexOf("npc_init") < 0 && parent.Controls[i].Name.IndexOf("npc_") >= 0)
                {
                    int controlNo = Convert.ToInt16(parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("npc_init") < 0)
                        thisNo = Convert.ToInt16(this.Tag);

                    if (controlNo > thisNo)
                        parent.Controls[i].Location = new Point(parent.Controls[i].Location.X, parent.Controls[i].Location.Y + 195);
                }
            }

            qs.textBox_Script.Focus();
        }        

        // 대사 삭제(중간단계삭제)
        public void Del_SubStep(string p_name, string p_number)
        {
            Quest_Unit parent = (Quest_Unit)this.Parent;

            Quest_Step delete_step = null;

            // 선택된 NPC 찾기
            for (int i = 0; i < this.Controls.Count; ++i)
            {
                if (this.Controls[i].Name == p_name)
                {
                    delete_step = (Quest_Step)this.Controls[i];
                    break;
                }
            }
            // 선택된 NPC가 없을 경우 그냥 리턴
            if (delete_step == null)
            {
                return;
            }
            else
            {   
                if (delete_step.Tag == null)
                {
                    for (int i = 0; i < this.Controls.Count; ++i)
                    {
                        if (this.Controls[i].Tag != null)
                        {
                            this.Controls[i].Location = new Point(this.Controls[i].Location.X, this.Controls[i].Location.Y - delete_step.Height);

                            int step_no = 0;
                            if (Int32.TryParse(this.Controls[i].Tag.ToString(), out step_no))
                            {
                                Quest_Step temp_step = (Quest_Step)this.Controls[i];
                                temp_step.Tag = step_no - 1;
                                temp_step.Name = "step_" + temp_step.Tag.ToString();
                                temp_step.textBox_StepNo.Text = array_alphabet[step_no - 1];
                            }
                        }
                    }                                    
                }
                else
                {
                    int temp = 0;
                    if (Int32.TryParse(p_number, out temp))
                    {
                        for (int i = 0; i < this.Controls.Count; ++i)
                        {
                            if (this.Controls[i].Tag != null && temp < Convert.ToInt32(this.Controls[i].Tag))
                            {
                                this.Controls[i].Location = new Point(this.Controls[i].Location.X, this.Controls[i].Location.Y - delete_step.Height);

                                int step_no = 0;
                                if (Int32.TryParse(this.Controls[i].Tag.ToString(), out step_no))
                                {
                                    Quest_Step temp_step = (Quest_Step)this.Controls[i];
                                    temp_step.Tag = step_no - 1;
                                    temp_step.Name = "step_" + temp_step.Tag.ToString();
                                    temp_step.textBox_StepNo.Text = array_alphabet[step_no - 1];
                                }
                            }
                        }
                    }                               
                }

                // 스텝 삭제로 인한 NPC 창 크기 및 그룹박스 크기 변경
                this.Size = new Size(this.Size.Width, this.Size.Height - delete_step.Height);
                groupBox_NPC.Size = new Size(groupBox_NPC.Size.Width, groupBox_NPC.Size.Height - delete_step.Height);

                // 스텝 삭제로 인한 유닛 창 크기 및 그룹박스 크기 변경
                parent.Size = new Size(parent.Size.Width, parent.Size.Height - delete_step.Height);
                parent.groupBox_Unit.Height = parent.groupBox_Unit.Height - delete_step.Height;

                //하위 NPC 위치 변경
                for (int i = 0; i < parent.Controls.Count; ++i)
                {
                    if (parent.Controls[i].Name.IndexOf("npc_init") < 0 && parent.Controls[i].Name.IndexOf("npc_") >= 0)
                    {
                        int controlNo = Convert.ToInt16(parent.Controls[i].Tag);
                        int thisNo = -1;

                        if (this.Name.IndexOf("npc_init") < 0)
                            thisNo = Convert.ToInt16(this.Tag);

                        if (controlNo > thisNo)
                            parent.Controls[i].Location = new Point(parent.Controls[i].Location.X, parent.Controls[i].Location.Y - delete_step.Height);
                    }
                }

                delete_step.Dispose();
                --Step_Count;
            }
        }

        // NPC컨트롤에 속해있는 Step 배열 반환 함수(저장시에 사용 - Step 뿐만 아닌 속해있는 모든 컨트롤 수만큼 반환)
        public Quest_Step[] Get_Step()
        {
            Quest_Step[] ret = new Quest_Step[this.Controls.Count];

            for (int i = 0; i < this.Controls.Count; ++i)
            {
                if (this.Controls[i].Name.IndexOf("groupBox_") == -1)
                    ret[i] = (Quest_Step)this.Controls[i];
            }

            Quick_Sort(ret, 0, ret.Length-1);
            return ret;
        }

        // NPC 이름 반환 함수
        public string Get_NPCName()
        {
            return this.textBox_NPCName.Text;
        }

        // 실행번호 반환 함수
        public string Get_ExeNo()
        {
            return this.textBox_ExecuteNo.Text;
        }

        // 스텝 추가 버튼 클릭 이벤트
        private void button_SubAdd_Click(object sender, EventArgs e)
        {
            Add_SubStep();
        }

        // 스텝 삭제 버튼 클릭 이벤트
        private void button_SubDel_Click(object sender, EventArgs e)
        {
            if (Step_Count > 1)
            {
                Quest_Unit parent = (Quest_Unit)this.Parent;

                Quest_Step delete_step = (Quest_Step)this.Controls["step_" + (Step_Count - 1)];                

                if (Step_Count > 0)
                {
                    this.Size = new Size(this.Size.Width, this.Size.Height - delete_step.Height);
                    groupBox_NPC.Size = new Size(groupBox_NPC.Size.Width, groupBox_NPC.Size.Height - delete_step.Height);

                    parent.Size = new Size(parent.Size.Width, parent.Size.Height - delete_step.Height);
                    parent.groupBox_Unit.Height = parent.groupBox_Unit.Height - delete_step.Height;

                    for (int i = 0; i < parent.Controls.Count; ++i)
                    {
                        if (parent.Controls[i].Name.IndexOf("npc_init") < 0 && parent.Controls[i].Name.IndexOf("npc_") >= 0)
                        {
                            int controlNo = Convert.ToInt16(parent.Controls[i].Tag);
                            int thisNo = -1;

                            if (this.Name.IndexOf("npc_init") < 0)
                                thisNo = Convert.ToInt16(this.Tag);

                            if (controlNo > thisNo)
                                parent.Controls[i].Location = new Point(parent.Controls[i].Location.X, parent.Controls[i].Location.Y - delete_step.Height);
                        }
                    }

                    this.Controls["step_" + (Step_Count - 1)].Dispose();

                    Step_Count--;
                }
            }
        }

        // NPC 창 클릭 이벤트 (기본 정보 페이지에서 설정한 NPC리스트를 가져온다)
        private void textBox_NPCName_Click(object sender, EventArgs e)
        {
            frm_Main main = (frm_Main)ParentForm;
            SelectNPC select = new SelectNPC(main.Get_NpcList());
            if (select.ShowDialog() == DialogResult.OK)
                textBox_NPCName.Text = select.selected_npc;
        }

        //꺽쇠 체크
        private void Check_GT(TextBox p_txt, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    p_txt.Text = p_txt.Text.Substring(0, p_txt.Text.Length - 1);
                    p_txt.SelectionStart = p_txt.Text.Length;
                }
            }
        }

        private void textBox_NPCName_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }
        
    }
}