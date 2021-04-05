using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FileToByte {
    public partial class FTBWin : Form {
        public FTBWin() {
            InitializeComponent();
        }

        public static string    path;
        public static int       line;

        private void Close_Click(object sender, EventArgs e) {
            Application.ExitThread();
        }

        private void minimize_Click(object sender, EventArgs e) {
            WindowState = FormWindowState.Minimized;
        }

        private void Copy_Click(object sender, EventArgs e) {
            if (ftb.Text.Length > 0) {
                Clipboard.SetText(ftb.Text);
            }
        }

        private void Open_Click(object sender, EventArgs e) {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.ShowDialog();
            file_path.Text = ofd.FileName;
            path = ofd.FileName;
        }
    
        private void Convert_Click(object sender, EventArgs e) {
            if (array_name.Text.Length > 0 && file_path.Text.Length > 0) {
                byte[] fileBytes = File.ReadAllBytes(path);

                StringBuilder sb = new StringBuilder(fileBytes.Length);

                sb.AppendLine($"BYTE {array_name.Text}[/*{fileBytes.Length}*/] = {{");
                for (int i = 0; i < fileBytes.Length; i++) {
                    line++;
                    sb.Append($"\t0x{fileBytes[i]:X2}, ");

                    if (line == 10) {
                        sb.Append("\n");
                        line = 0;
                    }
                }
                sb.Append("\n};");
                ftb.Text += sb.ToString();
            } else {
                MessageBox.Show("Array name or File path missing!", "Missing arguments.");
            }
        }

        private bool mouseDown;
        private Point lastLocation;

        private void titlebar_MouseDown(object sender, MouseEventArgs e) {
            mouseDown = true;
            lastLocation = e.Location;
        }

        private void titlebar_MouseMove(object sender, MouseEventArgs e) {
            if (mouseDown) {
                this.Location = new Point(
                    (this.Location.X - lastLocation.X) + e.X, (this.Location.Y - lastLocation.Y) + e.Y);

                this.Update();
            }
        }

        private void titlebar_MouseUp(object sender, MouseEventArgs e) {
            mouseDown = false;
        }
    }
}
