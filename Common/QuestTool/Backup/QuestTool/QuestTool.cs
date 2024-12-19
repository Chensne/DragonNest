using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DNNpc;
using DNQuest;
using System.Security;
using System.Threading;

namespace QuestTool
{
    public partial class QuestTool : Form
    {
        private Quest_Common qc = new Quest_Common();
        public eCOUNTRY SELECTED_COUNTRY = eCOUNTRY.DEFAULT;
        public bool bAUTOSAVE = false;
        private SaveProgress m_SaveProgress = null;
        private bool _ForceSaveCancel = false;
        public bool ForceSaveCancel
        {
            get { return _ForceSaveCancel; }
            set { _ForceSaveCancel = value; }
        }

        //아이콘 표스 리스트박스 아이템 클래스
        public class OptionListItem
        {
            public enum IconType
            {
                Keys,
                Security,                
            }

            public IconType eIconType = IconType.Keys; 
            private string text = "";

            public OptionListItem(string text)
            {
                this.text = text;
            }

            public OptionListItem(string text, IconType icon)
                : this(text)
            {
                this.eIconType = icon;
            }

            public string Text
            {
                get { return this.text; }
                set { this.text = value; }
            }
        }

        public QuestTool()
        {
            InitializeComponent();
            timer4AutoSave.Interval = 120000;
            timer4AutoSave.Tick += new EventHandler(timer4AutoSave_Tick);
            //Timer를 시작할 때 동작시킵니다.
            //temp 저장 로직 완성 후, 주석 제거
            //timer4AutoSave.Enabled = true;
        }

        void timer4AutoSave_Tick(object sender, EventArgs e)
        {
            if (this.ActiveMdiChild != null) // 활성화된 자식 창을 저장함
            {
                if (ActiveMdiChild.GetType().ToString() == "DNQuest.frm_Main") //퀘스트 창일 경우
                {
                    DNQuest.frm_Main temp = (DNQuest.frm_Main)ActiveMdiChild;
                    temp.sAToolStripMenuItem_Click(sender, e);
                }
                else // NPC 창일 경우
                {
                    DNNpc.DNNpc temp = (DNNpc.DNNpc)ActiveMdiChild;
                    temp.saveToolStripMenuItem_Click(sender, e);
                }
            }
        }

        // NPC 새로 만들기 메뉴 클릭 이벤트
        private void nPCToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DNNpc.DNNpc npc = new DNNpc.DNNpc(); 

            npc.MdiParent = this;
            npc.WindowState = FormWindowState.Maximized;
            npc.Text = "NPC-New";            
            npc.Show();
        }

        // Quest 새로 만들기 메뉴 클릭 이벤트
        private void questToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DNQuest.frm_Main quest = new frm_Main();

            quest.MdiParent = this;
            quest.WindowState = FormWindowState.Maximized;
            quest.Text = "Quest-New";            
            quest.Show();
        }

        // 기본 퀘스트 로드 함수(R:\Resource 폴더로부터 로딩)
        private void QuestTool_Load(object sender, EventArgs e)
        {
            comboBox_Culture.SelectedIndex = 0;
            SELECTED_COUNTRY = eCOUNTRY.DEFAULT;
        }

        public string getQuestSavePath()
        {
            return qc.getQuestPathByCountry(SELECTED_COUNTRY);
        }

        public string getNPCSavePath()
        {
            return qc.getNPCPathByCountry(SELECTED_COUNTRY);
        }

        // 퀘스트 리스트 초기화
        private void init_QuestList()
        {
            string searchString = @"";
            List<string> searchQuestType = new List<string>();
            if (checkBox1.Checked) searchQuestType.Add("mq");
            if (checkBox2.Checked) searchQuestType.Add("sq");
            if (checkBox3.Checked) searchQuestType.Add("dq");
            if (checkBox4.Checked) searchQuestType.Add("gq");
            if (checkBox5.Checked) searchQuestType.Add("nq");

            if (searchQuestType.Count > 0)
            {
                searchString += @"(";
                foreach (string word in searchQuestType)
                    searchString = searchString + word + @"|";

                searchString = searchString.Remove(searchString.Length - 1) + @")+";
            }
            if (comboBox1.Text.Length > 0)
                searchString += @"[0-9a-zA-Z_]*(" + comboBox1.Text + @")+";
            setListBox(listBox_Quest, getQuestSavePath(), searchString);
        }

        // NPC 리스트 초기화
        private void init_NPCList()
        {
            init_NPCList("");
        }

        private void init_NPCList(string search)
        {
            string searchString = comboBox4.Text.Trim();
            setListBox(listBox_NPC, getNPCSavePath(), search + @"[0-9a-zA-Z_]+(" + searchString + @")+");
        }

        private void setListBox(ListBox target, string path, string search)
        {
            target.Items.Clear();

            DirectoryInfo directory = new DirectoryInfo(path);
            
            if (!directory.Exists)
            {
                String error = path.ToString();
                error += "\n경로를 찾을수가 없습니다. ";
                MessageBox.Show( error );
                return;
            }
      
            FileInfo[] files = directory.GetFiles("*.xml");

            System.Text.RegularExpressions.Regex regex = new System.Text.RegularExpressions.Regex(search);

            foreach (FileInfo file in files)
            {    
                if (regex.IsMatch(file.Name))
                {
                    if (file.IsReadOnly)
                    {
                        target.Items.Add(new OptionListItem(file.Name, OptionListItem.IconType.Keys ));
                    }
                    else
                    {
                        target.Items.Add(new OptionListItem(file.Name, OptionListItem.IconType.Security));
                    }
                }
            }
        }

        // 언어 콤보박스 변경 이벤트
        private void comboBox_Culture_SelectedIndexChanged(object sender, EventArgs e)
        {
            SELECTED_COUNTRY = (eCOUNTRY)comboBox_Culture.SelectedIndex;
            init_QuestList();
            init_NPCList();
        }

        // 퀘스트 리스트 박스 더블클릭 이벤트
        private void listBox_Quest_DoubleClick(object sender, EventArgs e)
        {
            if (listBox_Quest.SelectedItem != null)
            {
                OptionListItem item = (OptionListItem)listBox_Quest.SelectedItem;
                DNQuest.frm_Main quest = new frm_Main(qc.getQuestPathByCountry(SELECTED_COUNTRY), item.Text);
                quest.MdiParent = this;
                quest.WindowState = FormWindowState.Maximized;                
                quest.Show(); 
            }
        }

        // NPC 리스트 박스 더블클릭 이벤트
        private void listBox_NPC_DoubleClick(object sender, EventArgs e)
        {
            if (listBox_NPC.SelectedItem != null)
            {
                OptionListItem item = (OptionListItem)listBox_NPC.SelectedItem;
                DNNpc.DNNpc npc = new DNNpc.DNNpc(qc.getNPCPathByCountry(SELECTED_COUNTRY), item.Text);
                npc.MdiParent = this;
                npc.WindowState = FormWindowState.Maximized;
                npc.Show();
            }
        }

        // 퀘스트 리스트 박스 아이템 드로우 이벤트
        private void listBox_Quest_DrawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index >= 0)
            {
                OptionListItem item = (OptionListItem)listBox_Quest.Items[e.Index];
                Bitmap bmOffscreen = new Bitmap(e.Bounds.Width, e.Bounds.Height);
                Graphics gfx = Graphics.FromImage(bmOffscreen);
                bool selected = ((e.State & DrawItemState.Selected) == DrawItemState.Selected);
                Brush brBackground = SystemBrushes.Menu;
                Rectangle bounds = new Rectangle(0, 0, e.Bounds.Width, e.Bounds.Height);
                Brush itemTextBrush = SystemBrushes.ControlText;

                if (selected)
                {
                    itemTextBrush = SystemBrushes.HighlightText;
                    brBackground = new SolidBrush(Color.FromArgb(128, SystemColors.Highlight));
                    gfx.FillRectangle(brBackground, bounds);
                    e.DrawFocusRectangle();
                }
                else
                {
                    brBackground = new SolidBrush(Color.White);
                    gfx.FillRectangle(brBackground, bounds);
                }

                gfx.DrawString(item.Text, Font, itemTextBrush, 20.0F, 5.0F);

                if (item.eIconType == OptionListItem.IconType.Keys)
                {
                    gfx.DrawIcon(Properties.Resources.keys, 1, (e.Bounds.Height - Properties.Resources.keys.Height) / 2);
                }
                else if (item.eIconType == OptionListItem.IconType.Security)
                {
                    gfx.DrawIcon(Properties.Resources.security, 1, (e.Bounds.Height - Properties.Resources.security.Height) / 2);
                }

                e.Graphics.DrawImageUnscaled(bmOffscreen, e.Bounds.X, e.Bounds.Y);

                bmOffscreen.Dispose();
            }
        }

        // NPC 리스트 박스 아이템 드로우 이벤트
        private void listBox_NPC_DrawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index >= 0)
            {
                OptionListItem item = (OptionListItem)listBox_NPC.Items[e.Index];

                Bitmap bmOffscreen = new Bitmap(e.Bounds.Width, e.Bounds.Height);

                Graphics gfx = Graphics.FromImage(bmOffscreen);

                bool selected = ((e.State & DrawItemState.Selected) == DrawItemState.Selected);

                Brush brBackground = SystemBrushes.Menu;

                Rectangle bounds = new Rectangle(0, 0, e.Bounds.Width, e.Bounds.Height);

                Brush itemTextBrush = SystemBrushes.ControlText;

                if (selected) {

                    itemTextBrush = SystemBrushes.HighlightText;
                    brBackground = new SolidBrush(Color.FromArgb(128, SystemColors.Highlight));
                    gfx.FillRectangle(brBackground, bounds);
                    e.DrawFocusRectangle();
                } 
                else {
                    brBackground = new SolidBrush(Color.White);
                    gfx.FillRectangle(brBackground, bounds);
                }

                gfx.DrawString(item.Text, Font, itemTextBrush, 20.0F, 5.0F);

                if (item.eIconType == OptionListItem.IconType.Keys)
                {
                    gfx.DrawIcon(Properties.Resources.keys, 1, (e.Bounds.Height - Properties.Resources.keys.Height) / 2);
                }
                else if (item.eIconType == OptionListItem.IconType.Security)
                {
                    gfx.DrawIcon(Properties.Resources.security, 1, (e.Bounds.Height - Properties.Resources.security.Height) / 2);
                }

                e.Graphics.DrawImageUnscaled(bmOffscreen, e.Bounds.X, e.Bounds.Y);

                bmOffscreen.Dispose();
            }
        }

        private delegate void _del_UpdateSaveProgress(int count, int total, string filename, bool success, string message);
        private void _UpdateSaveProgress(int count, int total, string filename, bool success, string message)
        {
            if(null == m_SaveProgress)
                return;

            m_SaveProgress.UpdateData(count, total, filename, success, message);
        }

        private void _StartSaveProgress()
        {
            m_SaveProgress = new SaveProgress(this);
            m_SaveProgress.ShowDialog();

            m_SaveProgress.Close();
            m_SaveProgress = null;
        }

        // 테스트 메뉴 클릭이벤트(테스트시에만 사용)
        private void testToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DNNpc.DNNpc.NpcAllSaveCheck = false;
            frm_Main.QuestAllSaveCheck= false;

            int failcount = 0;
            int savecount = 0;
            if (listBox_Quest.Items.Count > 0)
            {
                Thread progress = new Thread(new ThreadStart(_StartSaveProgress));
                progress.Start();

                foreach (OptionListItem item in listBox_Quest.Items)
                {
                    if(true == _ForceSaveCancel)
                    {
                        break;
                    }

                    try
                    {
                        using (DNQuest.frm_Main quest = new frm_Main(qc.DefaultQuestPath, item.Text))
                        {
                            string message = "";
                            quest.MdiParent = this;
                            quest.WindowState = FormWindowState.Minimized;
                            quest.Hide_Menu();
                            quest.Show();
                            bool success = quest.TestScript(ref message);
                            if (false == success)
                                ++failcount;

                            quest.Close();
                            ++savecount;

                            Invoke(new _del_UpdateSaveProgress(_UpdateSaveProgress), savecount, listBox_Quest.Items.Count, item.Text, success, message);

                        }
                    }
                    catch (Exception exception)
                    {
                        MessageBox.Show(exception.Message);
                    }
                }

                progress.Abort();
            }
            MessageBox.Show("Total : " + listBox_Quest.Items.Count.ToString() + " / " + "Success : " + (savecount - failcount) + " / " + "Fail : " + failcount.ToString(), "Save Result !!!", MessageBoxButtons.OK, MessageBoxIcon.Information);

            _ForceSaveCancel = false;
        }

        private void NpcAllSaveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DNNpc.DNNpc.NpcAllSaveCheck = false;
            frm_Main.QuestAllSaveCheck = false;

            if (listBox_NPC.Items.Count > 0)
            {
                foreach (OptionListItem item in listBox_NPC.Items)
                {
                    using (DNNpc.DNNpc npc = new DNNpc.DNNpc(qc.getNPCPathByCountry(this.SELECTED_COUNTRY), item.Text))
                    {
                        npc.MdiParent = this;
                        npc.WindowState = FormWindowState.Maximized;
                        npc.Hide_Menu();
                        npc.Show();
                        npc.TestScript();
                        npc.Close();
                    }
                }
            }
        }  

        // 종료 메뉴 클릭 이벤트
        private void questTool종료QToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        // NPC 열기 메뉴 버튼 클릭 이벤트
        private void OpenNPCToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openfile = new OpenFileDialog();
            openfile.Filter = "*.xml|*.XML";
            openfile.InitialDirectory = qc.getNPCPathByCountry(this.SELECTED_COUNTRY); // R:\를 기본 폴더로 지정

            DirectoryInfo di = new DirectoryInfo(openfile.InitialDirectory);

            if (!di.Exists) // 기본 폴더 유부 확인
            {
                openfile.InitialDirectory = @"C:\"; // 기본 폴더 없을 경우 C:\를 기본으로 지정
            }

            if (openfile.ShowDialog() == DialogResult.OK)
            {
                DNNpc.DNNpc npc = new DNNpc.DNNpc(qc.getNPCPathByCountry(this.SELECTED_COUNTRY), openfile.FileName);
                npc.MdiParent = this;
                npc.WindowState = FormWindowState.Maximized;                
                npc.Show();
            }
        }

        // 퀘스트 열기 메뉴 버튼 클릭 이벤트
        private void OpenQuestToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openfile = new OpenFileDialog();
            openfile.Filter = "*.xml|*.XML";
            openfile.InitialDirectory = @"R:\GameRes\Resource\Script\Talk_Quest";

            DirectoryInfo di = new DirectoryInfo(openfile.InitialDirectory);

            if (!di.Exists)
            {
                openfile.InitialDirectory = @"C:\";
            }

            if (openfile.ShowDialog() == DialogResult.OK)
            {
                DNQuest.frm_Main quest = new DNQuest.frm_Main(qc.getQuestPathByCountry(SELECTED_COUNTRY), openfile.FileName);
                quest.MdiParent = this;
                quest.WindowState = FormWindowState.Maximized;
                quest.Show();
            }
        }

        // 저장 메뉴 클릭 이벤트
        private void SaveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.ActiveMdiChild != null) // 활성화된 자식 창을 저장함
            {
                if (ActiveMdiChild.GetType().ToString() == "DNQuest.frm_Main") //퀘스트 창일 경우
                {
                    DNQuest.frm_Main temp = (DNQuest.frm_Main)ActiveMdiChild;
                    temp.sAToolStripMenuItem_Click(sender, e);
                }
                else // NPC 창일 경우
                {
                    DNNpc.DNNpc temp = (DNNpc.DNNpc)ActiveMdiChild;
                    temp.saveToolStripMenuItem_Click(sender, e);
                }
            }
        }

        // 창 닫기 메뉴 클릭 이벤트 
        private void 닫기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //활성화 된 자식 창 닫기
            if (ActiveMdiChild != null)
                ActiveMdiChild.Close();
        }

        private void questTool정보ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutForm about = new AboutForm(this);
            about.ShowDialog();
        }

        // 창닫기 및 저장 버튼 활성화를 위한 메뉴바 클릭 이벤트 
        // 메뉴를 클릭 할때 마다 자식창의 유무를 파악하여 자식창이 존재하고
        // 활성화 된 자식창이 있을 경우에만 창닫기와 저장 버튼을 활성화 함
        private void menuStrip1_Click(object sender, EventArgs e) 
        {
            if (MdiChildren.Length > 0 && ActiveMdiChild != null)
            {
                닫기ToolStripMenuItem.Enabled = true;
                SaveToolStripMenuItem.Enabled = true;
            }
            else
            {
                닫기ToolStripMenuItem.Enabled = false;
                SaveToolStripMenuItem.Enabled = false;
            }
        }

        private void PathToolStripMenuItem_Click(object sender, EventArgs e)
        {
            QuestPathSetting form_Path = new QuestPathSetting();

            form_Path.ShowIcon = false;
            form_Path.StartPosition = FormStartPosition.CenterParent;
            form_Path.FormBorderStyle = FormBorderStyle.FixedToolWindow;
            form_Path.ShowInTaskbar = false;

            if (form_Path.ShowDialog() == DialogResult.OK)
            {
                qc.ReloadPath();

                comboBox_Culture.SelectedIndex = 0;
                SELECTED_COUNTRY = eCOUNTRY.DEFAULT;
            }
        }

        private void button1_Enter(object sender, EventArgs e)
        {
            this.checkBox1.Checked = false;
            this.checkBox2.Checked = false;
            this.checkBox3.Checked = false;
            this.checkBox4.Checked = false;
            this.checkBox5.Checked = false;
            this.comboBox1.Text = "";
        }

        private void button13_Enter(object sender, EventArgs e)
        {
            this.comboBox4.Text = "";
            init_NPCList();
        }

        private void comboBox1_Enter(object sender, EventArgs e)
        {
            init_QuestList();
        }

        private void comboBox4_Enter(object sender, EventArgs e)
        {
            init_NPCList();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            init_NPCList("n0\\d\\d");
        }

        private void button5_Click(object sender, EventArgs e)
        {
            init_NPCList("n1\\d\\d");
        }

        private void button6_Click(object sender, EventArgs e)
        {
            init_NPCList("n2\\d\\d");
        }

        private void button7_Click(object sender, EventArgs e)
        {
            init_NPCList("n3\\d\\d");
        }

        private void button8_Click(object sender, EventArgs e)
        {
            init_NPCList("n4\\d\\d");
        }

        private void button9_Click(object sender, EventArgs e)
        {
            init_NPCList("n5\\d\\d");
        }

        private void button10_Click(object sender, EventArgs e)
        {
            init_NPCList("n6\\d\\d");
        }

        private void button11_Click(object sender, EventArgs e)
        {
            init_NPCList("n7\\d\\d");
        }

        private void button12_Click(object sender, EventArgs e)
        {
            init_NPCList("n8\\d\\d");
        }

        private void button14_Click(object sender, EventArgs e)
        {
            init_NPCList("n9\\d\\d");
        }

        private void yesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bAUTOSAVE = true;
        }

        private void noToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bAUTOSAVE = false;
        }

        private void listBox_Quest_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && e.Clicks == 1)
            {
                if (listBox_Quest.Items.Count == 0)
                    return;
                
                DoDragDrop(
                    qc.getQuestPathByCountry(SELECTED_COUNTRY) + @"\" + ((OptionListItem)listBox_Quest.Items[listBox_Quest.IndexFromPoint(e.X, e.Y)]).Text
                    , DragDropEffects.All);
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            init_QuestList();
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            init_QuestList();
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            init_QuestList();
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            init_QuestList();
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            init_QuestList();
        }
    }
}