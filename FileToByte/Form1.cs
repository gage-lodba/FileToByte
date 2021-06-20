using System;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace FileToByte {
    public partial class FtbWin : Form {
        public FtbWin() {
            InitializeComponent();
        }

        public static string    Path;
        public static int       Line;

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
            Path = ofd.FileName;
        }
    
        private void Convert_Click(object sender, EventArgs e) {
            if (array_name.Text.Length > 0 && file_path.Text.Length > 0) {
                var fileBytes = File.ReadAllBytes(Path);
                var sb = new StringBuilder(fileBytes.Length);

                sb.AppendLine($"unsigned char {array_name.Text}[/*{fileBytes.Length}*/] = {{");
                foreach (var t in fileBytes)
                {
                    Line++;
                    sb.Append($"\t0x{t:X2}, ");

                    if (Line != 10) 
                        continue;

                    sb.Append("\n");
                    Line = 0;
                }
                sb.Length -= 2;
                sb.Append("\n};");
                ftb.Text += sb.ToString();
            } else {
                MessageBox.Show(@"Array name or File path missing!", @"Missing arguments.");
            }
        }

        private bool _mouseDown;
        private Point _lastLocation;

        private void titlebar_MouseDown(object sender, MouseEventArgs e) {
            _mouseDown = true;
            _lastLocation = e.Location;
        }

        private void titlebar_MouseMove(object sender, MouseEventArgs e)
        {
            if (!_mouseDown) 
                return;

            this.Location = new Point((this.Location.X - _lastLocation.X) + e.X, (this.Location.Y - _lastLocation.Y) + e.Y);
            this.Update();
        }

        private void titlebar_MouseUp(object sender, MouseEventArgs e) {
            _mouseDown = false;
        }
    }
}
