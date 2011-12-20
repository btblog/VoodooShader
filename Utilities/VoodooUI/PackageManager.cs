﻿/*
 * This file is part of the Voodoo Shader Framework.
 *
 * Copyright (c) 2010-2011 by Sean Sube
 *
 * The Voodoo Shader Framework is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this program; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 US
 *
 * Support and more information may be found at
 *   http://www.voodooshader.com
 * or by contacting the lead developer at
 *   peachykeen@voodooshader.com
 */
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using VoodooSharp;

namespace VoodooUI
{
    public partial class PackageManager : Form
    {
        bool m_CancelNav;
        ProgressDialog m_ProgressForm;

        public PackageManager()
        {
            InitializeComponent();

            cBrowserDesc.Navigating += new WebBrowserNavigatingEventHandler(webBrowser1_Navigating);

            RefreshTree();
        }

        void webBrowser1_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            e.Cancel = m_CancelNav;
            if (m_CancelNav)
            {
                System.Diagnostics.Process.Start(e.Url.ToString());
            }
            m_CancelNav = true;
        }

        void RefreshTree()
        {
            List<String> packages = FindPackages(GlobalRegistry.Instance.Path);

            cPackageTree.Nodes.Clear();
            foreach (String package in packages)
            {
                cPackageTree.Nodes.Add(package);
            }
        }

        List<String> FindPackages(String root)
        {
            List<String> packages = new List<String>();
            string[] subdirs = Directory.GetDirectories(root);

            foreach (string subdir in subdirs)
            {
                if (String.Compare(subdir, "user", true) != 0 && Directory.Exists(Path.Combine(subdir, ".git")))
                {
                    packages.Add(Path.GetFileName(subdir));
                }
            }

            return packages;
        }

        void ClonePackage(String source, String branch, String package)
        {
            try
            {
                String fullpath = Path.Combine(GlobalRegistry.Instance.Path, package);

                if (Directory.Exists(fullpath))
                {
                    MessageBox.Show(String.Format("Unable to install package {0}, directory already exists.", package));
                } else {
                    m_ProgressForm = new ProgressDialog();
                    m_ProgressForm.Show(this);

                    m_ProgressForm.ShellExecute(String.Format("git clone \"{0}\" \"{1}\"", source, fullpath));

                    String wd = Directory.GetCurrentDirectory();
                    Directory.SetCurrentDirectory(fullpath);
                    m_ProgressForm.ShellExecute(String.Format("git checkout \"{0}\"", branch));
                    Directory.SetCurrentDirectory(wd);
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("Error installing package:\n" + exc.Message);
            }
        }

        void UpdatePackage(String package)
        {
            try
            {
                String fullpath = Path.Combine(GlobalRegistry.Instance.Path, package);

                if (!Directory.Exists(fullpath))
                {
                    MessageBox.Show(String.Format("Unable to update package {0}, directory does not exist.", package));
                }
                else
                {
                    String wd = Directory.GetCurrentDirectory();
                    Directory.SetCurrentDirectory(fullpath);
                    m_ProgressForm = new ProgressDialog();
                    m_ProgressForm.Show(this);
                    m_ProgressForm.ShellExecute("git reset --hard");
                    m_ProgressForm.ShellExecute("git pull");
                    Directory.SetCurrentDirectory(wd);
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("Error updating package:\n" + exc.Message);
            }
        }

        void RemovePackage(String package)
        {
            try
            {
                String fullpath = Path.Combine(GlobalRegistry.Instance.Path, package);

                if (Directory.Exists(fullpath))
                {
                    Directory.Delete(fullpath, true);
                }
            }
            catch (System.Exception exc)
            {
                MessageBox.Show("Error removing package:\n" + exc.Message);            	
            }
        }

        void StartupCheck()
        {
            if (!GlobalRegistry.Exists)
            {
                dFolderBrowser.Description = "Select Voodoo Shader installation path.";
                if (dFolderBrowser.ShowDialog() == DialogResult.OK)
                {
                    GlobalRegistry.Instance.Path = dFolderBrowser.SelectedPath;
                    GlobalRegistry.Instance.Write();
                }
            }

            if (!ExistsInPath("git.exe") && !ExistsInPath("git.cmd"))
            {
                m_ProgressForm = new ProgressDialog();
                m_ProgressForm.WriteLine("Unable to locate git for package management.");

                WebClient client = new WebClient();
                client.DownloadProgressChanged += new DownloadProgressChangedEventHandler(client_DownloadProgressChanged);
                client.DownloadFileCompleted += new System.ComponentModel.AsyncCompletedEventHandler(client_DownloadFileCompleted);

                client.DownloadFileAsync(new Uri("http://msysgit.googlecode.com/files/Git-1.7.8-preview20111206.exe"), "msysgit.exe");
                m_ProgressForm.Show(this);
                this.Enabled = false;
                m_ProgressForm.WriteLine("Downloading git installer...");
            }
        }

        void client_DownloadProgressChanged(object sender, DownloadProgressChangedEventArgs e)
        {
            m_ProgressForm.Progress = e.ProgressPercentage;
        }

        void client_DownloadFileCompleted(object sender, System.ComponentModel.AsyncCompletedEventArgs e)
        {
            m_ProgressForm.WriteLine("Done downloading git installer.");

            SHA1 verify = SHA1.Create();
            byte[] hash = verify.ComputeHash(File.ReadAllBytes("msysgit.exe"));
            byte[] goal = { 0x75, 0x8b, 0xd1, 0xbb, 0xb9, 0x12, 0x73, 0x24, 0x87, 0xd7, 0xb4, 0x24, 0x0f, 0x7d, 0xe3, 0x3b, 0xbb, 0xab, 0x18, 0x48 };
            bool valid = true;
            for (int i = 0; i < hash.Length; ++i)
            {
                if (goal[i] != hash[i]) valid = false;
            }

            if (!valid)
            {
                m_ProgressForm.WriteLine("The git installer appears to be corrupt.");
                m_ProgressForm.WriteLine("You will need to download and install git manually.");
                Process.Start("http://code.google.com/p/msysgit/downloads/detail?name=Git-1.7.8-preview20111206.exe");

                m_ProgressForm.AllowClose = true;
                this.Enabled = true;
            }
            else
            {
                m_ProgressForm.WriteLine("Launching git installer...");
                Process.Start("msysgit.exe").WaitForExit();
                m_ProgressForm.WriteLine("Git installer has completed.");

                m_ProgressForm.AllowClose = true;
                this.Enabled = true;
            }
        }

        public static bool ExistsInPath(string fileName)
        {
            if (GetFullPath(fileName) != null)
                return true;
            return false;
        }

        public static string GetFullPath(string fileName)
        {
            if (File.Exists(fileName))
                return Path.GetFullPath(fileName);

            var values = Environment.GetEnvironmentVariable("PATH");
            foreach (var path in values.Split(';'))
            {
                var fullPath = Path.Combine(path, fileName);
                if (File.Exists(fullPath))
                    return fullPath;
            }
            return null;
        }

        private void FormShown(object sender, EventArgs e)
        {
            StartupCheck();
        }

        private void PackageSelect(object sender, TreeViewEventArgs e)
        {
            String pack = e.Node.Text;
            toolStripComboBox1.Items[0] = pack;
            toolStripComboBox1.SelectedIndex = 0;

            String fullpath = Path.Combine(GlobalRegistry.Instance.Path, pack);
            String manifest = Path.Combine(fullpath, "package.xml");
            if (File.Exists(manifest))
            {
                XmlValidator xv = new XmlValidator();
                Package package = xv.ValidateObject<Package>(manifest);

                m_CancelNav = false;
                if (package != null)
                {
                    cBrowserDesc.DocumentText = package.Languages[0].Description;
                }
                else
                {
                    cBrowserDesc.DocumentText = "Unable to load package info.";
                }
            }
        }

        private void ButtonInstall(object sender, EventArgs e)
        {
            PackageDialog details = new PackageDialog();
            if (details.ShowDialog() == DialogResult.OK)
            {
                if (String.Compare(details.Path, "user") == 0)
                {
                    MessageBox.Show(String.Format("Unable to install package {0}, illegal path.", details.Path));
                }
                else
                {
                    if (String.IsNullOrEmpty(details.Branch))
                    {
                        details.Branch = "master";
                    }

                    ClonePackage(details.Origin, details.Branch, details.Path);
                }
            }

            RefreshTree();
        }

        private void ButtonUpdate(object sender, EventArgs e)
        {
            if (toolStripComboBox1.SelectedIndex == 0)
            {
                if (cPackageTree.SelectedNode != null)
                {
                    UpdatePackage(cPackageTree.SelectedNode.Text);
                }
            }
            else
            {
                foreach (TreeNode node in cPackageTree.Nodes)
                {
                    UpdatePackage(node.Text);
                }

            }

            RefreshTree();
        }

        private void ButtonRemove(object sender, EventArgs e)
        {
            if (toolStripComboBox1.SelectedIndex == 0)
            {
                if (cPackageTree.SelectedNode != null)
                {
                    String path = cPackageTree.SelectedNode.Text;
                    if (MessageBox.Show(String.Format("Are you sure you want to remove package {0}?", path), "Remove All", MessageBoxButtons.YesNo, MessageBoxIcon.Stop) == DialogResult.Yes)
                    {
                        RemovePackage(path);
                    }
                }
            }
            else
            {
                if (MessageBox.Show("Are you sure you want to remove all installed packages?", "Remove All", MessageBoxButtons.YesNo, MessageBoxIcon.Stop) == DialogResult.Yes)
                {
                    foreach (TreeNode node in cPackageTree.Nodes)
                    {
                        RemovePackage(node.Text);
                    }
                }
            }

            RefreshTree();
        }
    }
}
