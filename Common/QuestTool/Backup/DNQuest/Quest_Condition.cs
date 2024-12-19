using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_Condition : UserControl
    {
        int Condition_Count = 0;
        int Execution_Count = 0;
        int Hunt_Count = 0;

        public Quest_Condition()
        {
            InitializeComponent();
        }

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

        private void button_ConditionAdd_Click(object sender, EventArgs e)
        {
            switch (comboBox_Condition.SelectedIndex)
            {
                case 0:
                    Conditions.Condition_Level cl = new DNQuest.Conditions.Condition_Level();
                    cl.Location = new Point(10, (31 * Condition_Count) + 40);
                    cl.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cl);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 1:
                    Conditions.Condition_GeneralItem cg = new DNQuest.Conditions.Condition_GeneralItem();
                    cg.Location = new Point(10, (31 * Condition_Count) + 40);
                    cg.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cg);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 2:
                    Conditions.Condition_QuestItem cqi = new DNQuest.Conditions.Condition_QuestItem();
                    cqi.Location = new Point(10, (31 * Condition_Count) + 40);
                    cqi.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cqi);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 3:
                    Conditions.Condition_Class cc = new DNQuest.Conditions.Condition_Class();
                    cc.Location = new Point(10, (31 * Condition_Count) + 40);
                    cc.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cc);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 4:
                    Conditions.Condition_Prob cp = new DNQuest.Conditions.Condition_Prob();
                    cp.Location = new Point(10, (31 * Condition_Count) + 40);
                    cp.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cp);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 5:
                    Conditions.Condition_Quest cq = new DNQuest.Conditions.Condition_Quest();
                    cq.Location = new Point(10, (31 * Condition_Count) + 40);
                    cq.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(cq);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 6:
                    Conditions.Condition_Compelete ccom = new DNQuest.Conditions.Condition_Compelete();
                    ccom.Location = new Point(10, (31 * Condition_Count) + 40);
                    ccom.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(ccom);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 7:
                    Conditions.Condition_Inven ci = new DNQuest.Conditions.Condition_Inven();
                    ci.Location = new Point(10, (31 * Condition_Count) + 40);
                    ci.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(ci);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 8:
                    Conditions.Condition_Custum ccustum = new DNQuest.Conditions.Condition_Custum();
                    ccustum.Location = new Point(10, (31 * Condition_Count) + 40);
                    ccustum.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(ccustum);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 9:
                    Conditions.Condition_QuestCnt qcnt = new DNQuest.Conditions.Condition_QuestCnt();
                    qcnt.Location = new Point(10, (31 * Condition_Count) + 40);
                    qcnt.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(qcnt);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
                case 10:
                    Conditions.Condition_StageConstructionLevel stageConstructionLevel = new DNQuest.Conditions.Condition_StageConstructionLevel();
                    stageConstructionLevel.Location = new Point(10, (31 * Condition_Count) + 40);
                    stageConstructionLevel.label_No.Text = Condition_Count.ToString();
                    groupBox_Condition.Controls.Add(stageConstructionLevel);
                    Resize_Condition_Add();
                    ++Condition_Count;
                    break;
            }
        }

        private void button_CondtionDel_Click(object sender, EventArgs e)
        {
            if (Condition_Count > 0)
            {
                groupBox_Condition.Controls[groupBox_Condition.Controls.Count - 1].Dispose();
                --Condition_Count;
                Resize_Condition_Del();
            }
        }

        private void Resize_Condition_Add()
        {
            this.Height = this.Height + 31;
            groupBox_Condition.Size = new Size(groupBox_Condition.Width, groupBox_Condition.Height + 31);
            groupBox_Execution.Location = new Point(groupBox_Execution.Location.X, groupBox_Execution.Location.Y + 31);
            groupBox_Detail.Location = new Point(groupBox_Detail.Location.X, groupBox_Detail.Location.Y + 31);
            groupBox_Unit.Height = groupBox_Unit.Height + 31;
        }

        private void Resize_Condition_Del()
        {
            this.Height = this.Height - 31;
            groupBox_Condition.Size = new Size(groupBox_Condition.Width, groupBox_Condition.Height - 31);
            groupBox_Execution.Location = new Point(groupBox_Execution.Location.X, groupBox_Execution.Location.Y - 31);
            groupBox_Detail.Location = new Point(groupBox_Detail.Location.X, groupBox_Detail.Location.Y - 31);
            groupBox_Unit.Height = groupBox_Unit.Height - 31;
        }

        private void Resize_Execution_Add()
        {
            this.Height = this.Height + 31;
            groupBox_Execution.Size = new Size(groupBox_Execution.Width, groupBox_Execution.Height + 31);            
            groupBox_Detail.Location = new Point(groupBox_Detail.Location.X, groupBox_Detail.Location.Y + 31);
            groupBox_Unit.Height = groupBox_Unit.Height + 31;
        }

        private void Resize_Execution_Del()
        {
            this.Height = this.Height - 31;
            groupBox_Execution.Size = new Size(groupBox_Execution.Width, groupBox_Execution.Height - 31);
            groupBox_Detail.Location = new Point(groupBox_Detail.Location.X, groupBox_Detail.Location.Y - 31);
            groupBox_Unit.Height = groupBox_Unit.Height - 31;
        }

        /// <summary>
        /// 실행문 추가, switch문 의 case 는 design time에서 등록해 주는 combo 리스트의 배열 번호와 동일
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_ExeAdd_Click(object sender, EventArgs e)
        {
            switch ( comboBox_Execution.Text )
            {
                case "[아이템] 일반아이템삭제":
                    Executions.Exe_ItemDel exe_itemdel = new DNQuest.Executions.Exe_ItemDel();
                    exe_itemdel.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_itemdel.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_itemdel);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[아이템] 퀘스트아이템삭제":
                    Executions.Exe_QuestItemDel exe_qidel = new DNQuest.Executions.Exe_QuestItemDel();
                    exe_qidel.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_qidel.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_qidel);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[아이템] 일반아이템추가":
                    Executions.Exe_ItemAdd exe_itemadd = new DNQuest.Executions.Exe_ItemAdd();
                    exe_itemadd.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_itemadd.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_itemadd);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[아이템] 퀘스트아이템추가":
                    Executions.Exe_QuestItemAdd exe_qiadd = new DNQuest.Executions.Exe_QuestItemAdd();
                    exe_qiadd.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_qiadd.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_qiadd);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[임무] 몬스터사냥추가":
                    if (Hunt_Count < 30)
                    {
                        Executions.Exe_MonsterAdd exe_monsteradd = new DNQuest.Executions.Exe_MonsterAdd();
                        exe_monsteradd.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_monsteradd.label_No.Text = Execution_Count.ToString();
                        groupBox_Execution.Controls.Add(exe_monsteradd);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        ++Hunt_Count;
                    }
                    else
                    {
                        MessageBox.Show("심볼수집 추가, 몬스터 사냥 추가, 아이템수집 추가, 스테이지 클리어 랭크 조건 실행문은 30개 이상 생성할 수 없습니다!", "실행문", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    break;
                case "[임무] 아이템(일반/퀘스트)수집추가":
                    if (Hunt_Count < 30)
                    {
                        Executions.Exe_ItemColAdd exe_itemcoladd = new DNQuest.Executions.Exe_ItemColAdd();
                        exe_itemcoladd.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_itemcoladd.label_No.Text = Execution_Count.ToString();
                        groupBox_Execution.Controls.Add(exe_itemcoladd);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        ++Hunt_Count;
                    }
                    else
                    {
                        MessageBox.Show("심볼수집 추가, 몬스터 사냥 추가, 아이템수집 추가, 스테이지 클리어 랭크 조건 실행문은 30개 이상 생성할 수 없습니다!", "실행문", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    break;
                case "[임무] 등록 모든 수집 테스트가 완료 됐을 때 실행설정":
                    Executions.Exe_AllHunting exe_all = new DNQuest.Executions.Exe_AllHunting();
                    exe_all.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_all.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_all);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 퀘스트 받기":
                    Executions.Exe_QuestAdd exe_questadd = new DNQuest.Executions.Exe_QuestAdd();
                    exe_questadd.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_questadd.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_questadd);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 일일 퀘스트 받기":
                    Executions.Exe_AddDailyQuest exe_addDailyQuest = new DNQuest.Executions.Exe_AddDailyQuest();
                    exe_addDailyQuest.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_addDailyQuest.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_addDailyQuest);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 퀘스트 스텝 변경":
                    Executions.Exe_QuestStepMod exe_queststep = new DNQuest.Executions.Exe_QuestStepMod();
                    exe_queststep.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_queststep.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_queststep);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 퀘스트 완료":
                    Executions.Exe_QuestComplete exe_questcom = new DNQuest.Executions.Exe_QuestComplete();
                    exe_questcom.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_questcom.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_questcom);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 퀘스트 저널 변경":
                    Executions.Exe_JournalMod exe_journal = new DNQuest.Executions.Exe_JournalMod();
                    exe_journal.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_journal.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_journal);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[실행] 메세지 출력":
                    Executions.Exe_Message exe_message = new DNQuest.Executions.Exe_Message();
                    exe_message.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_message.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_message);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[실행] 대사창 출력":
                    Executions.Exe_Script exe_script = new DNQuest.Executions.Exe_Script();
                    exe_script.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_script.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_script);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[실행] 확률 택일 실행":
                    Executions.Exe_ProbChoice exe_choice = new DNQuest.Executions.Exe_ProbChoice();
                    exe_choice.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_choice.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_choice);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[아이템] 유저 돈 소모시키기":
                    Executions.Exe_MoneyDel exe_moneyDel = new DNQuest.Executions.Exe_MoneyDel();
                    exe_moneyDel.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_moneyDel.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_moneyDel);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "인게임 무비":
                    break;
                case "[실행] 조건체크 실행":
                    Executions.Exe_Condition exe_condition = new DNQuest.Executions.Exe_Condition();
                    exe_condition.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_condition.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_condition);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[임무] 모든 등록된 사냥, 수집 삭제":
                    Executions.Exe_AddDel exe_del = new DNQuest.Executions.Exe_AddDel();
                    exe_del.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_del.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_del);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[기타] 커스텀 코드":
                    Executions.Exe_Custum exe_custum = new DNQuest.Executions.Exe_Custum();
                    exe_custum.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_custum.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_custum);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "Return 코드 추가":
                    Executions.Exe_Return exe_return = new DNQuest.Executions.Exe_Return();
                    exe_return.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_return.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_return);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "인벤토리 Full 표시창 출력":
                    Executions.Exe_FullInven exe_fullinven = new DNQuest.Executions.Exe_FullInven();
                    exe_fullinven.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_fullinven.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_fullinven);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "퀘스트 Full 표시창 출력":
                    Executions.Exe_FullQuest exe_fullquest = new DNQuest.Executions.Exe_FullQuest();
                    exe_fullquest.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_fullquest.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_fullquest);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 보상창출력(보여주기)":
                    Executions.Exe_Recompense_View exe_recompense_view = new DNQuest.Executions.Exe_Recompense_View();
                    exe_recompense_view.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_recompense_view.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_recompense_view);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[상태] 보상창출력(보상지급)":
                    Executions.Exe_Recompense exe_recompense = new DNQuest.Executions.Exe_Recompense();
                    exe_recompense.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_recompense.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_recompense);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[기타] 코멘트":
                    Executions.Exe_Comment exe_commente = new DNQuest.Executions.Exe_Comment();
                    exe_commente.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_commente.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_commente);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[아이템] 일반/퀘스트아이템추가":
                    Executions.Exe_AddItemAndQuestItem exe_addItemANDQuestItem = new DNQuest.Executions.Exe_AddItemAndQuestItem();
                    exe_addItemANDQuestItem.Location = new Point(10, (31 * Execution_Count) + 40);
                    exe_addItemANDQuestItem.label_No.Text = Execution_Count.ToString();
                    groupBox_Execution.Controls.Add(exe_addItemANDQuestItem);
                    Resize_Execution_Add();
                    ++Execution_Count;
                    break;
                case "[임무] 스테이지 클리어 랭크 조건":    
                    if (Hunt_Count < 30)
                    {
                        Executions.Exe_CheckStageClearRank exe_checkStageClearRank = new DNQuest.Executions.Exe_CheckStageClearRank();
                        exe_checkStageClearRank.Location = new Point(10, (31 * Execution_Count) + 40);
                        exe_checkStageClearRank.label_No.Text = Execution_Count.ToString();
                        groupBox_Execution.Controls.Add(exe_checkStageClearRank);
                        Resize_Execution_Add();
                        ++Execution_Count;
                        ++Hunt_Count;
                    }
                    else
                    {
                        MessageBox.Show("심볼수집 추가, 몬스터 사냥 추가, 아이템수집 추가, 스테이지 클리어 랭크 조건 실행문은 30개 이상 생성할 수 없습니다!", "실행문", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    break;
            }
        }

        /// <summary>
        /// 실행문 삭제
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_ExeDel_Click(object sender, EventArgs e)
        {
            if (Execution_Count > 0)
            {
                string arr_Hunt = "DNQuest.Executions.Exe_MonsterAdd;DNQuest.Executions.Exe_ItemColAdd;DNQuest.Executions.Exe_SymbolColAdd";

                if (arr_Hunt.IndexOf(groupBox_Execution.Controls[groupBox_Execution.Controls.Count - 1].ToString()) > -1)
                {
                    --Hunt_Count;
                }

                groupBox_Execution.Controls[groupBox_Execution.Controls.Count - 1].Dispose();
                --Execution_Count;               

                Resize_Execution_Del();
            }
        }

        public void Set_Condition_Memo(string p_memo)
        {
            textBox_Detail.Text = p_memo;
        }

        public void Add_Condition(Control p_condition)
        {
            p_condition.Location = new Point(10, (31 * Condition_Count) + 40);
            p_condition.Controls["label_No"].Text = Condition_Count.ToString();
            groupBox_Condition.Controls.Add(p_condition);
            Resize_Condition_Add();
            ++Condition_Count;
        }

        public void Add_Execution(Control p_execution)
        {
            p_execution.Location = new Point(10, (31 * Execution_Count) + 40);
            p_execution.Controls["label_No"].Text = Execution_Count.ToString();
            groupBox_Execution.Controls.Add(p_execution);
            Resize_Execution_Add();
            ++Execution_Count;
        }

        private void textBox_Detail_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    textBox_Detail.Text = textBox_Detail.Text.Substring(0, textBox_Detail.Text.Length - 1);
                    textBox_Detail.SelectionStart = textBox_Detail.Text.Length;
                }
            }
        }
    }
}
