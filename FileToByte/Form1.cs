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

namespace FileToBytee
{
    public partial class FTB : Form
    {
        public FTB()
        {
            InitializeComponent();
        }

        public static string Path;
        public static int Line;

        private bool _mouseDown;
        private Point _lastLocation;

        private void FTB_Load(object sender, EventArgs e)
        {
            Close.FlatAppearance.BorderSize = 0;
            Minimize.FlatAppearance.BorderSize = 0;
            Open.FlatAppearance.BorderSize = 0;
            Convert.FlatAppearance.BorderSize = 0;
            CTC.FlatAppearance.BorderSize = 0;

            Close.FlatAppearance.BorderColor = Color.FromArgb(0, 0, 0, 0);
            Minimize.FlatAppearance.BorderColor = Color.FromArgb(0, 0, 0, 0);
            Open.FlatAppearance.BorderColor = Color.FromArgb(0, 0, 0, 0);
            Convert.FlatAppearance.BorderColor = Color.FromArgb(0, 0, 0, 0);
            CTC.FlatAppearance.BorderColor = Color.FromArgb(0, 0, 0, 0);
        }

        private void Close_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void Minimize_Click(object sender, EventArgs e)
        {
            WindowState = FormWindowState.Minimized;
        }

        private void Open_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.ShowDialog();
            filePath.Text = ofd.FileName;
            Path = ofd.FileName;
        }

        private void Convert_Click(object sender, EventArgs e)
        {
            if (arrayName.Text.Length > 0 && arrayName.Text.Length > 0)
            {
                var fileBytes = File.ReadAllBytes(Path);
                var sb = new StringBuilder(fileBytes.Length);

                sb.AppendLine($"unsigned char {arrayName.Text}[/*{fileBytes.Length}*/] = {{");
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
                result.Text = sb.ToString();
            }
            else
            {
                MessageBox.Show(@"Array name or File path missing!", @"Missing arguments.");
            }
        }

        private void CTC_Click(object sender, EventArgs e)
        {
            if (result.Text.Length > 0)
            {
                Clipboard.SetText(result.Text);
            }
        }

        private void titlebar_MouseDown(object sender, MouseEventArgs e)
        {
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

        private void titlebar_MouseUp(object sender, MouseEventArgs e)
        {
            _mouseDown = false;
        }
    }
}