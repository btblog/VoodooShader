﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace VoodooGUI
{
    public partial class MainForm : Form
    {

        public MainForm()
        {
            InitializeComponent();
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
                "Using this function will retrieve a list of supported applications from the Voodoo server and scan your hard drive for installed programs.\n"+
                "This uses file names only and will only scan folders you can open; file contents will not be accessed and no file data will be sent to the server.\n"+
                "This may take some time if you have many files. A list of possible programs will be shown when the scan is done.\n"+
                "Are you sure you want to run this scan?", 
                "Confirm Scan", MessageBoxButtons.YesNo, MessageBoxIcon.Information) == DialogResult.Yes)
            {
                Form scanWindow = new ScanForm();
                scanWindow.ShowDialog();
            }
        }
    }
}