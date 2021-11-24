using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NoDesktopWMUI
{
    public partial class NoDesktopWMain : Form
    {
        private Timer _timer = new Timer();
        public NoDesktopWMain()
        {
            InitializeComponent();
        }

        //TODO watch in background thread
        private void startBtn_Click(object sender, EventArgs e)
        {
            this.statusLabel.Text = "Running";

            FindAndInjectToExplorer();
        }

        private void stopBtn_Click(object sender, EventArgs e)
        {
            this.statusLabel.Text = "Not Running";
        }

        private void infoBtn_Click(object sender, EventArgs e)
        {

        }

        private void FindAndInjectToExplorer()
        {
            string dllPath;

            if(IntPtr.Size == 8)
            {
                //x64
                dllPath = Path.Combine(Directory.GetCurrentDirectory(), "x64", "NoDesktopWM.dll");
            }
            else
            {
                //x86
                dllPath = Path.Combine(Directory.GetCurrentDirectory(), "x86", "NoDesktopWM.dll");
            }

            string explorerName = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Windows),"explorer.exe").ToLower();
            Process[] processlist = Process.GetProcesses();

            if (!File.Exists(dllPath))
            {
                MessageBox.Show(string.Format("{0} not found!",dllPath),"Error");
                return;
            }

            foreach(Process process in processlist)
            {
                try
                {
                    if(process.ProcessName == "explorer")
                    {
                        bool alreadyAttached = false;
                        var dllList = process.Modules;
                        foreach(ProcessModule module in dllList)
                        {
                            if(module.ModuleName == "NoDesktopWM.dll")
                            {
                                alreadyAttached = true;
                                break;
                            }
                        }

                        if (alreadyAttached)
                        {
                            Trace.WriteLine("already attached");
                            continue;
                        }

                   
                        Injection.inject(dllPath, process);
                    }
                }
                catch
                {

                }
            }
        }

    }
}
