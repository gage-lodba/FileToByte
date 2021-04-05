
namespace FileToByte
{
    partial class FTBWin
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FTBWin));
            this.titlebar = new System.Windows.Forms.Panel();
            this.minimize = new System.Windows.Forms.Button();
            this.close = new System.Windows.Forms.Button();
            this.title = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel4 = new System.Windows.Forms.Panel();
            this.panel5 = new System.Windows.Forms.Panel();
            this.ftb = new System.Windows.Forms.RichTextBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.Copy = new System.Windows.Forms.Button();
            this.Convert = new System.Windows.Forms.Button();
            this.Open = new System.Windows.Forms.Button();
            this.file_path = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.array_name = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.titlebar.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel4.SuspendLayout();
            this.panel5.SuspendLayout();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // titlebar
            // 
            this.titlebar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(45)))));
            this.titlebar.Controls.Add(this.minimize);
            this.titlebar.Controls.Add(this.close);
            this.titlebar.Controls.Add(this.title);
            this.titlebar.Dock = System.Windows.Forms.DockStyle.Top;
            this.titlebar.Location = new System.Drawing.Point(1, 1);
            this.titlebar.Name = "titlebar";
            this.titlebar.Size = new System.Drawing.Size(798, 30);
            this.titlebar.TabIndex = 5;
            this.titlebar.MouseDown += new System.Windows.Forms.MouseEventHandler(this.titlebar_MouseDown);
            this.titlebar.MouseMove += new System.Windows.Forms.MouseEventHandler(this.titlebar_MouseMove);
            this.titlebar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.titlebar_MouseUp);
            // 
            // minimize
            // 
            this.minimize.FlatAppearance.BorderSize = 0;
            this.minimize.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.minimize.Font = new System.Drawing.Font("Webdings", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.minimize.Location = new System.Drawing.Point(734, 0);
            this.minimize.Name = "minimize";
            this.minimize.Size = new System.Drawing.Size(30, 30);
            this.minimize.TabIndex = 12;
            this.minimize.Text = "0";
            this.minimize.UseVisualStyleBackColor = true;
            this.minimize.Click += new System.EventHandler(this.minimize_Click);
            // 
            // close
            // 
            this.close.FlatAppearance.BorderSize = 0;
            this.close.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.close.Font = new System.Drawing.Font("Webdings", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.close.Location = new System.Drawing.Point(770, 0);
            this.close.Name = "close";
            this.close.Size = new System.Drawing.Size(30, 30);
            this.close.TabIndex = 11;
            this.close.Text = "r";
            this.close.UseVisualStyleBackColor = true;
            this.close.Click += new System.EventHandler(this.Close_Click);
            // 
            // title
            // 
            this.title.AutoSize = true;
            this.title.Location = new System.Drawing.Point(8, 8);
            this.title.Name = "title";
            this.title.Size = new System.Drawing.Size(68, 15);
            this.title.TabIndex = 10;
            this.title.Text = "File to byte";
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.panel2.Controls.Add(this.panel4);
            this.panel2.Controls.Add(this.panel3);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(1, 31);
            this.panel2.Name = "panel2";
            this.panel2.Padding = new System.Windows.Forms.Padding(15, 5, 15, 15);
            this.panel2.Size = new System.Drawing.Size(798, 568);
            this.panel2.TabIndex = 9;
            // 
            // panel4
            // 
            this.panel4.BackColor = System.Drawing.Color.DimGray;
            this.panel4.Controls.Add(this.panel5);
            this.panel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel4.Location = new System.Drawing.Point(15, 80);
            this.panel4.Name = "panel4";
            this.panel4.Padding = new System.Windows.Forms.Padding(1);
            this.panel4.Size = new System.Drawing.Size(768, 473);
            this.panel4.TabIndex = 2;
            // 
            // panel5
            // 
            this.panel5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.panel5.Controls.Add(this.ftb);
            this.panel5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel5.Location = new System.Drawing.Point(1, 1);
            this.panel5.Name = "panel5";
            this.panel5.Padding = new System.Windows.Forms.Padding(3);
            this.panel5.Size = new System.Drawing.Size(766, 471);
            this.panel5.TabIndex = 0;
            // 
            // ftb
            // 
            this.ftb.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.ftb.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ftb.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ftb.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ftb.ForeColor = System.Drawing.Color.White;
            this.ftb.Location = new System.Drawing.Point(3, 3);
            this.ftb.Name = "ftb";
            this.ftb.Size = new System.Drawing.Size(760, 465);
            this.ftb.TabIndex = 0;
            this.ftb.Text = "";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.Copy);
            this.panel3.Controls.Add(this.Convert);
            this.panel3.Controls.Add(this.Open);
            this.panel3.Controls.Add(this.file_path);
            this.panel3.Controls.Add(this.label2);
            this.panel3.Controls.Add(this.array_name);
            this.panel3.Controls.Add(this.label1);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel3.Location = new System.Drawing.Point(15, 5);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(768, 75);
            this.panel3.TabIndex = 0;
            // 
            // Copy
            // 
            this.Copy.FlatAppearance.BorderColor = System.Drawing.Color.DimGray;
            this.Copy.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Copy.Font = new System.Drawing.Font("Roboto", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.Copy.Location = new System.Drawing.Point(518, 47);
            this.Copy.Name = "Copy";
            this.Copy.Size = new System.Drawing.Size(250, 23);
            this.Copy.TabIndex = 6;
            this.Copy.Text = "Copy";
            this.Copy.UseVisualStyleBackColor = true;
            this.Copy.Click += new System.EventHandler(this.Copy_Click);
            // 
            // Convert
            // 
            this.Convert.FlatAppearance.BorderColor = System.Drawing.Color.DimGray;
            this.Convert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Convert.Font = new System.Drawing.Font("Roboto", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.Convert.Location = new System.Drawing.Point(256, 47);
            this.Convert.Name = "Convert";
            this.Convert.Size = new System.Drawing.Size(250, 23);
            this.Convert.TabIndex = 5;
            this.Convert.Text = "Convert";
            this.Convert.UseVisualStyleBackColor = true;
            this.Convert.Click += new System.EventHandler(this.Convert_Click);
            // 
            // Open
            // 
            this.Open.FlatAppearance.BorderColor = System.Drawing.Color.DimGray;
            this.Open.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Open.Font = new System.Drawing.Font("Roboto", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.Open.Location = new System.Drawing.Point(0, 47);
            this.Open.Margin = new System.Windows.Forms.Padding(0);
            this.Open.Name = "Open";
            this.Open.Size = new System.Drawing.Size(250, 23);
            this.Open.TabIndex = 4;
            this.Open.Text = "Open";
            this.Open.UseVisualStyleBackColor = true;
            this.Open.Click += new System.EventHandler(this.Open_Click);
            // 
            // file_path
            // 
            this.file_path.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.file_path.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.file_path.ForeColor = System.Drawing.Color.White;
            this.file_path.Location = new System.Drawing.Point(256, 18);
            this.file_path.Name = "file_path";
            this.file_path.Size = new System.Drawing.Size(512, 23);
            this.file_path.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(260, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 15);
            this.label2.TabIndex = 2;
            this.label2.Text = "File path";
            // 
            // array_name
            // 
            this.array_name.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.array_name.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.array_name.ForeColor = System.Drawing.Color.White;
            this.array_name.Location = new System.Drawing.Point(0, 18);
            this.array_name.Name = "array_name";
            this.array_name.Size = new System.Drawing.Size(250, 23);
            this.array_name.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 15);
            this.label1.TabIndex = 0;
            this.label1.Text = "Array name";
            // 
            // FTBWin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.ClientSize = new System.Drawing.Size(800, 600);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.titlebar);
            this.Font = new System.Drawing.Font("Roboto", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ForeColor = System.Drawing.Color.White;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FTBWin";
            this.Padding = new System.Windows.Forms.Padding(1);
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.titlebar.ResumeLayout(false);
            this.titlebar.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            this.panel5.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Panel titlebar;
        private System.Windows.Forms.Label title;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button close;
        private System.Windows.Forms.TextBox file_path;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox array_name;
        private System.Windows.Forms.Button Open;
        private System.Windows.Forms.Button Copy;
        private System.Windows.Forms.Button Convert;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.RichTextBox ftb;
        private System.Windows.Forms.Button minimize;
    }
}

