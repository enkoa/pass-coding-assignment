
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import java.io.*;

import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;

public class BHL_Fun extends JPanel implements ActionListener  {
	JButton button1;
	JLabel label1;
	Font appFont;
	JPanel daPanel1, southPanel, innerP;	
	public BHL_Fun () {
		appFont = new Font("Old English Text MT", Font.PLAIN, 30);
		this.setLayout(new BorderLayout());
		
		daPanel1 = new JPanel();
		southPanel = new JPanel();
		southPanel.setLayout(new BorderLayout());

		button1 = new JButton("Click Me");
		button1.setFont(appFont);
		
		innerP = new JPanel();
		innerP.add(button1);
		southPanel.add(innerP, BorderLayout.SOUTH);
		
		add(daPanel1);
		add(southPanel, BorderLayout.SOUTH);

		button1.addActionListener(this);
		
	}
	// action listener method
	
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == button1) {
			try {
				int n = 8;
				JPanel[] manyPanel = new JPanel[n];
				ImageIcon im = new ImageIcon(new ImageIcon("BHL_SaxGuy.gif").getImage().getScaledInstance(100, 80, Image.SCALE_DEFAULT));						JPanel imagePanel = new JPanel();
				imagePanel.setLayout(new GridLayout(n,0));
				
				for (int i = 0; i < n; i ++) {
					manyPanel[i] = new JPanel();
					manyPanel[i].setLayout(new FlowLayout(FlowLayout.LEFT));
					for(int j = 0; j < n*n; j ++) {
						manyPanel[i].add(new JLabel(im));
					}
					imagePanel.add(manyPanel[i]);
				}
				add(imagePanel, BorderLayout.CENTER);
				playSound();
			} catch(Exception me) {
				System.out.println(me.toString());
			}
			button1.setText("No going back :)");
			button1.removeActionListener(this);
		}
	}
	public void playSound() {
		File sax = new File("BHL_theSax.wav");
    		 try {
       			Clip dclip = AudioSystem.getClip();
			dclip.open(AudioSystem.getAudioInputStream(sax));
			dclip.start();
			dclip.loop(Clip.LOOP_CONTINUOUSLY);	
    		} catch(Exception ex) {
      			  System.out.println("Error.");
 
 		}
	}
}