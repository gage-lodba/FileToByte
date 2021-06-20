
namespace FileToBytee
{
    partial class FTB
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FTB));
            this.titlebar = new System.Windows.Forms.Panel();
            this.content = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            this.Close = new System.Windows.Forms.Button();
            this.Minimize = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.arrayName = new System.Windows.Forms.TextBox();
            this.filePath = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.Open = new System.Windows.Forms.Button();
            this.Convert = new System.Windows.Forms.Button();
            this.CTC = new System.Windows.Forms.Button();
            this.result = new System.Windows.Forms.RichTextBox();
            this.titlebar.SuspendLayout();
            this.content.SuspendLayout();
            this.SuspendLayout();
            // 
            // titlebar
            // 
            this.titlebar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.titlebar.Controls.Add(this.Minimize);
            this.titlebar.Controls.Add(this.Close);
            this.titlebar.Controls.Add(this.label1);
            this.titlebar.Dock = System.Windows.Forms.DockStyle.Top;
            this.titlebar.Location = new System.Drawing.Point(1, 1);
            this.titlebar.Name = "titlebar";
            this.titlebar.Padding = new System.Windows.Forms.Padding(5, 0, 0, 0);
            this.titlebar.Size = new System.Drawing.Size(798, 30);
            this.titlebar.TabIndex = 0;
            this.titlebar.MouseDown += new System.Windows.Forms.MouseEventHandler(this.titlebar_MouseDown);
            this.titlebar.MouseMove += new System.Windows.Forms.MouseEventHandler(this.titlebar_MouseMove);
            this.titlebar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.titlebar_MouseUp);
            // 
            // content
            // 
            this.content.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(16)))), ((int)(((byte)(16)))), ((int)(((byte)(16)))));
            this.content.Controls.Add(this.result);
            this.content.Controls.Add(this.CTC);
            this.content.Controls.Add(this.Convert);
            this.content.Controls.Add(this.Open);
            this.content.Controls.Add(this.label3);
            this.content.Controls.Add(this.filePath);
            this.content.Controls.Add(this.arrayName);
            this.content.Controls.Add(this.label2);
            this.content.Dock = System.Windows.Forms.DockStyle.Fill;
            this.content.Location = new System.Drawing.Point(1, 31);
            this.content.Name = "content";
            this.content.Padding = new System.Windows.Forms.Padding(10, 5, 10, 10);
            this.content.Size = new System.Drawing.Size(798, 568);
            this.content.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(68, 14);
            this.label1.TabIndex = 0;
            this.label1.Text = "File To Byte";
            // 
            // Close
            // 
            this.Close.FlatAppearance.BorderSize = 0;
            this.Close.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Close.Font = new System.Drawing.Font("Webdings", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.Close.Location = new System.Drawing.Point(768, 0);
            this.Close.Name = "Close";
            this.Close.Size = new System.Drawing.Size(30, 30);
            this.Close.TabIndex = 0;
            this.Close.Text = "r";
            this.Close.UseVisualStyleBackColor = true;
            this.Close.Click += new System.EventHandler(this.Close_Click);
            // 
            // Minimize
            // 
            this.Minimize.FlatAppearance.BorderSize = 0;
            this.Minimize.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Minimize.Font = new System.Drawing.Font("Webdings", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.Minimize.Location = new System.Drawing.Point(732, 0);
            this.Minimize.Name = "Minimize";
            this.Minimize.Size = new System.Drawing.Size(30, 30);
            this.Minimize.TabIndex = 1;
            this.Minimize.Text = "0";
            this.Minimize.UseVisualStyleBackColor = true;
            this.Minimize.Click += new System.EventHandler(this.Minimize_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 5);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(70, 14);
            this.label2.TabIndex = 0;
            this.label2.Text = "Array name";
            // 
            // arrayName
            // 
            this.arrayName.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(24)))), ((int)(((byte)(24)))), ((int)(((byte)(24)))));
            this.arrayName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.arrayName.ForeColor = System.Drawing.Color.White;
            this.arrayName.Location = new System.Drawing.Point(13, 24);
            this.arrayName.Margin = new System.Windows.Forms.Padding(5);
            this.arrayName.Name = "arrayName";
            this.arrayName.Size = new System.Drawing.Size(250, 22);
            this.arrayName.TabIndex = 1;
            // 
            // filePath
            // 
            this.filePath.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(24)))), ((int)(((byte)(24)))), ((int)(((byte)(24)))));
            this.filePath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.filePath.ForeColor = System.Drawing.Color.White;
            this.filePath.Location = new System.Drawing.Point(269, 24);
            this.filePath.Name = "filePath";
            this.filePath.Size = new System.Drawing.Size(516, 22);
            this.filePath.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(268, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(54, 14);
            this.label3.TabIndex = 3;
            this.label3.Text = "File path";
            // 
            // Open
            // 
            this.Open.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(24)))), ((int)(((byte)(24)))), ((int)(((byte)(24)))));
            this.Open.FlatAppearance.BorderColor = System.Drawing.Color.DimGray;
            this.Open.FlatAppearance.BorderSize = 0;
            this.Open.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Open.Location = new System.Drawing.Point(13, 54);
            this.Open.Name = "Open";
            this.Open.Size = new System.Drawing.Size(250, 23);
            this.Open.TabIndex = 4;
            this.Open.Text = "Open";
            this.Open.UseVisualStyleBackColor = false;
            this.Open.Click += new System.EventHandler(this.Open_Click);
            // 
            // Convert
            // 
            this.Convert.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(24)))), ((int)(((byte)(24)))), ((int)(((byte)(24)))));
            this.Convert.FlatAppearance.BorderColor = System.Drawing.Color.DimGray;
            this.Convert.FlatAppearance.BorderSize = 0;
            this.Convert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Convert.Location = new System.Drawing.Point(269, 54);
            this.Convert.Name = "Convert";
            this.Convert.Size = new System.Drawing.Size(260, 23);
            this.Convert.TabIndex = 5;
            this.Convert.Text = "Convert";
            this.Convert.UseVisualStyleBackColor = false;
            this.Convert.Click += new System.EventHandler(this.Convert_Click);
            // 
            // CTC
            // 
            this.CTC.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(24)))), ((int)(((byte)(24)))), ((int)(((byte)(24)))));
            this.CTC.FlatAppearance.BorderColor = System.Drawing.Color.DimGray;
            this.CTC.FlatAppearance.BorderSize = 0;
            this.CTC.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.CTC.Location = new System.Drawing.Point(535, 54);
            this.CTC.Name = "CTC";
            this.CTC.Size = new System.Drawing.Size(250, 23);
            this.CTC.TabIndex = 6;
            this.CTC.Text = "Copy to clipboard";
            this.CTC.UseVisualStyleBackColor = false;
            this.CTC.Click += new System.EventHandler(this.CTC_Click);
            // 
            // result
            // 
            this.result.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(24)))), ((int)(((byte)(24)))), ((int)(((byte)(24)))));
            this.result.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.result.ForeColor = System.Drawing.Color.White;
            this.result.Location = new System.Drawing.Point(13, 83);
            this.result.Name = "result";
            this.result.Size = new System.Drawing.Size(772, 472);
            this.result.TabIndex = 7;
            this.result.Text = "";
            // 
            // FTB
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.ClientSize = new System.Drawing.Size(800, 600);
            this.Controls.Add(this.content);
            this.Controls.Add(this.titlebar);
            this.Font = new System.Drawing.Font("Roboto", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ForeColor = System.Drawing.Color.White;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FTB";
            this.Padding = new System.Windows.Forms.Padding(1);
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.FTB_Load);
            this.titlebar.ResumeLayout(false);
            this.titlebar.PerformLayout();
            this.content.ResumeLayout(false);
            this.content.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel titlebar;
        private System.Windows.Forms.Panel content;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button Minimize;
        private System.Windows.Forms.Button Close;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox arrayName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox filePath;
        private System.Windows.Forms.Button Open;
        private System.Windows.Forms.Button CTC;
        private System.Windows.Forms.Button Convert;
        private System.Windows.Forms.RichTextBox result;
    }
}

