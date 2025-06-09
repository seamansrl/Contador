namespace Comntador
{
    partial class Inicio
    {
        /// <summary>
        /// Variable del diseñador necesaria.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén usando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben desechar; false en caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido de este método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.Reset = new System.Windows.Forms.Button();
            this.Salir = new System.Windows.Forms.Button();
            this.Conectar = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.Contador = new System.Windows.Forms.Label();
            this.Terminal = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // Reset
            // 
            this.Reset.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(38)))), ((int)(((byte)(52)))), ((int)(((byte)(61)))));
            this.Reset.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(40)))), ((int)(((byte)(55)))));
            this.Reset.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Reset.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Reset.Location = new System.Drawing.Point(14, 274);
            this.Reset.Name = "Reset";
            this.Reset.Size = new System.Drawing.Size(531, 64);
            this.Reset.TabIndex = 0;
            this.Reset.Text = "Poner el contador a cero";
            this.Reset.UseVisualStyleBackColor = false;
            this.Reset.Click += new System.EventHandler(this.Reset_Click);
            // 
            // Salir
            // 
            this.Salir.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(38)))), ((int)(((byte)(52)))), ((int)(((byte)(61)))));
            this.Salir.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(40)))), ((int)(((byte)(55)))));
            this.Salir.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Salir.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Salir.Location = new System.Drawing.Point(12, 344);
            this.Salir.Name = "Salir";
            this.Salir.Size = new System.Drawing.Size(531, 64);
            this.Salir.TabIndex = 1;
            this.Salir.Text = "Salir";
            this.Salir.UseVisualStyleBackColor = false;
            this.Salir.Click += new System.EventHandler(this.Salir_Click);
            // 
            // Conectar
            // 
            this.Conectar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(38)))), ((int)(((byte)(52)))), ((int)(((byte)(61)))));
            this.Conectar.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(40)))), ((int)(((byte)(55)))));
            this.Conectar.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Conectar.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Conectar.Location = new System.Drawing.Point(12, 204);
            this.Conectar.Name = "Conectar";
            this.Conectar.Size = new System.Drawing.Size(531, 64);
            this.Conectar.TabIndex = 2;
            this.Conectar.Text = "Conectar";
            this.Conectar.UseVisualStyleBackColor = false;
            this.Conectar.Click += new System.EventHandler(this.Conectar_Click);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(231, 79);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(97, 22);
            this.label1.TabIndex = 3;
            this.label1.Text = "Contador";
            // 
            // Contador
            // 
            this.Contador.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Contador.Font = new System.Drawing.Font("Arial", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Contador.Location = new System.Drawing.Point(105, 119);
            this.Contador.Name = "Contador";
            this.Contador.Size = new System.Drawing.Size(345, 32);
            this.Contador.TabIndex = 4;
            this.Contador.Text = "---";
            this.Contador.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Terminal
            // 
            this.Terminal.BackColor = System.Drawing.Color.Black;
            this.Terminal.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Terminal.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.Terminal.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Terminal.Location = new System.Drawing.Point(0, 418);
            this.Terminal.Name = "Terminal";
            this.Terminal.Size = new System.Drawing.Size(557, 36);
            this.Terminal.TabIndex = 5;
            this.Terminal.Text = "Terminal";
            this.Terminal.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Inicio
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(53)))), ((int)(((byte)(72)))));
            this.ClientSize = new System.Drawing.Size(557, 454);
            this.Controls.Add(this.Terminal);
            this.Controls.Add(this.Contador);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Conectar);
            this.Controls.Add(this.Salir);
            this.Controls.Add(this.Reset);
            this.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "Inicio";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Contador";
            this.Load += new System.EventHandler(this.Inicio_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button Reset;
        private System.Windows.Forms.Button Salir;
        private System.Windows.Forms.Button Conectar;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label Contador;
        private System.Windows.Forms.Label Terminal;
    }
}

