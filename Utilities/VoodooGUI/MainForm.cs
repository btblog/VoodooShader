﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Microsoft.Win32;
using VoodooNetClasses;

namespace VoodooGUI
{
    public partial class MainForm : Form
    {
        [DllImport("kernel32.dll")]
        private static extern IntPtr LoadLibrary(String dllname);

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetProcAddress(IntPtr hModule, String procname);

        [DllImport("kernel32.dll")]
        private static extern bool FreeLibrary(IntPtr hModule);

        private delegate IntPtr InstallGlobalHook();
        private delegate void RemoveGlobalHook(IntPtr hHook);

        private IntPtr m_NativeModule;
        private IntPtr m_GlobalHook;

        private InstallGlobalHook m_InstallFunc;
        private RemoveGlobalHook m_RemoveFunc;

        private List<Hook> m_Hooks;

        public MainForm()
        {
            InitializeComponent();

            // Do the installation check
            LoadHooks();
            cHook_Table.AutoGenerateColumns = false;
            cHook_Table.DataSource = m_Hooks;

            m_NativeModule = LoadLibrary("Voodoo_HookStub.dll");
            if (m_NativeModule != IntPtr.Zero)
            {
                m_InstallFunc = (InstallGlobalHook)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_NativeModule, "InstallGlobalHook"), typeof(InstallGlobalHook));
                m_RemoveFunc = (RemoveGlobalHook)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_NativeModule, "RemoveGlobalHook"), typeof(RemoveGlobalHook));
                Menu_Hook_Enable(null, null);
            }
        }

        /// <summary>
        /// Retrieve a supported apps list and search for any installed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SearchSupported(object sender, EventArgs e)
        {
            // Verify they want to do this/security check
            if (MessageBox.Show(
                "Using this function will scan your hard drive for installed programs.\n"+
                "This uses file names only and will only scan folders you can open; file contents will not be accessed and no data will be sent to the server.\n"+
                "This may take some time if you have many files and may not be entirely accurate. A list of probable results will be shown when the scan is done.\n"+
                "Are you sure you want to run this scan?",
                "Confirm Scan", MessageBoxButtons.YesNo, MessageBoxIcon.Information) == DialogResult.Yes)
            {
                Form scanWindow = new ScanForm();
                scanWindow.ShowDialog();
            }
        }

        private void Menu_Hook_Add(object sender, EventArgs e)
        {
            Hook hook = new Hook();
            hook.Active = false;
            hook.Name = hook.Target = hook.Config = String.Empty;

            cHook_Table.EndEdit();
            this.BindingContext[cHook_Table.DataSource].SuspendBinding();
            m_Hooks.Add(hook);
            cHook_Table.DataSource = null;
            cHook_Table.DataSource = m_Hooks;
            this.BindingContext[cHook_Table.DataSource].ResumeBinding();
        }

        private void Menu_Hook_Remove(object sender, EventArgs e)
        {
            if (cHook_Table.SelectedRows.Count > 0)
            {
                cHook_Table.EndEdit();
                this.BindingContext[cHook_Table.DataSource].SuspendBinding();
                m_Hooks.RemoveAt(cHook_Table.SelectedRows[0].Index);
                cHook_Table.DataSource = null;
                cHook_Table.DataSource = m_Hooks;
                this.BindingContext[cHook_Table.DataSource].ResumeBinding();
            }
        }

        private void Form_OnResize(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Minimized)
            {
                Hide();

                cTrayIcon.BalloonTipTitle = "Voodoo UI Hidden";
                cTrayIcon.BalloonTipText = "Voodoo hooks will remain active while the UI is hidden.";
                cTrayIcon.ShowBalloonTip(2500);
            }
        }

        private void Notify_OnClick(object sender, EventArgs e)
        {
            if (!this.Visible)
            {
                Show();
                WindowState = FormWindowState.Normal;
            }
            else
            {
                this.Activate();
            }
        }

        private void Menu_Hook_Enable(object sender, EventArgs e)
        {
            if (m_InstallFunc != null)
            {
                m_GlobalHook = m_InstallFunc();
                if (m_GlobalHook != IntPtr.Zero)
                {
                    cMenu_Hook_Off.Visible = false;
                    cMenu_Hook_On.Visible = true;
                }
            }
        }

        private void Menu_Hook_Disable(object sender, EventArgs e)
        {
            if (m_RemoveFunc != null)
            {
                m_RemoveFunc(m_GlobalHook);
                m_GlobalHook = IntPtr.Zero;
                cMenu_Hook_Off.Visible = true;
                cMenu_Hook_On.Visible = false;
            }
        }

        private void MainForm_Close(object sender, FormClosedEventArgs e)
        {
            CommitHooks();

            if (m_GlobalHook != IntPtr.Zero)
            {
                Menu_Hook_Disable(null, null);
            }
        }

        private void LoadHooks()
        {
            try
            {
                RegistryKey hookRoot = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\VoodooShader\Hooks", RegistryKeyPermissionCheck.ReadSubTree);
                if (hookRoot != null)
                {
                    m_Hooks = VoodooRegistry.ReadHooks(hookRoot);
                    hookRoot.Close();
                }
                else
                {
                    m_Hooks = new List<Hook>();
                }
            }
            catch (System.Exception exc)
            {
                System.Windows.Forms.MessageBox.Show(exc.Message, "Error Reading Hooks", MessageBoxButtons.OK, MessageBoxIcon.Error);
                m_Hooks = new List<Hook>();
            }
        }

        private void CommitHooks()
        {
            try
            {
                RegistryKey voodooRoot = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\VoodooShader", RegistryKeyPermissionCheck.ReadWriteSubTree);
                if (voodooRoot == null)
                {
                    voodooRoot = Registry.CurrentUser.CreateSubKey(@"SOFTWARE\VoodooShader");
                }

                if (voodooRoot.OpenSubKey("Hooks") != null)
                {
                    voodooRoot.DeleteSubKeyTree("Hooks");
                }

                VoodooRegistry.Write(m_Hooks, voodooRoot);

                voodooRoot.Close();
            }
            catch (System.Exception exc)
            {
                System.Windows.Forms.MessageBox.Show(exc.Message, "Error Writing Hooks", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void CellClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex < 0 || e.RowIndex < 0) return;

            if (cHook_Table.Columns[e.ColumnIndex] == col_TargetFind)
            {
                dOpenFile.FilterIndex = 0;
                dOpenFile.Title = "Select Target Program";

                if (dOpenFile.ShowDialog() == DialogResult.OK)
                {
                    cHook_Table.Rows[e.RowIndex].Cells[e.ColumnIndex - 1].Value = dOpenFile.FileName;
                    CommitHooks();
                }
            }
            else if (cHook_Table.Columns[e.ColumnIndex] == col_ConfigFind)
            {
                dOpenFile.FilterIndex = 1;
                dOpenFile.Title = "Select Config File";

                if (dOpenFile.ShowDialog() == DialogResult.OK)
                {
                    cHook_Table.Rows[e.RowIndex].Cells[e.ColumnIndex - 1].Value = dOpenFile.FileName;
                    CommitHooks();
                }
            }
        }

        private void CellChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex < 0 || e.RowIndex < 0) return;

            CommitHooks();
        }
    }
}
