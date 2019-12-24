// bao le
// occc spring 2018
// advanced java
// click itpanel

import javax.swing.*;			// control
import java.awt.*;			// back end, listeners, etc 
import java.awt.event.*;		// event handers

import java.io.*;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
public class BHL_ClickIt extends JPanel implements MouseListener {
	private int row = 5,col = 5, x, y, z = 0;
	JButton[] listButton;
	JPanel[] listPanel;
	String daText = "Click me";
	private boolean music = false;
	public BHL_ClickIt() {
		Font appFont = new Font("Old English Text MT", Font.PLAIN, 20);
		this.setLayout(new GridLayout(row,col));

		listPanel = new JPanel[row*col];
		listButton = new JButton[row*col];
		for(int i = 0; i < listPanel.length; i++) {
			listPanel[i] = new JPanel();
			listPanel[i].setLayout(new BorderLayout());
			listButton[i] = new JButton();
			listButton[i].setFont(appFont);
			listPanel[i].add(listButton[i], BorderLayout.CENTER);
			add(listPanel[i]);
			
			listButton[i].addMouseListener(this);
		}
		x = (int)(Math.random() * (row * col));
		listButton[x].setText(daText); 
	}
	// MouseListener methods

	public void mouseClicked(MouseEvent me){}
	public void mouseEntered(MouseEvent me){
		if (me.getSource() == listButton[x]) {
			if (z > 10) {
				daText ="You Mad Bro?";
			}
			do {
				y = (int)(Math.random() * (row * col));
				listButton[y].setText(daText);
				z++;	
			} while( x==y );
			listButton[x].setText("");
			x = y;
		}
		if (!music ) {
			music = true;
			playSound();
		}
	}
	public void mouseExited(MouseEvent me){}
	public void mousePressed(MouseEvent me) {}
	public void mouseReleased(MouseEvent me){}

	// music

	public void playSound() {
		File sax = new File("BHL_Enjoy.wav");
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