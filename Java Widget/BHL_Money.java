// bao le
// occc spring 2018
// advanced java
// money class

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import java.text.DecimalFormat;
import java.text.NumberFormat;

import java.io.*;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
public class BHL_Money extends JPanel implements MouseMotionListener, MouseListener{
	JPanel [] manyPanel;	
	JLabel[] snoopLabel;
	JLabel cash, multiplier, daIm;
	private double money = 0, base = 0.01;
	private int currentX = 0, currentY = 0;
	private int Ucounter, counter;
	private int mult = 1, size = 25;
	private int panelNum = 0;
	
	JPanel northPanel, middlePanel, southPanel;	
	
	NumberFormat nf;
	Font appFont;
	ImageIcon im;
	private boolean changed = false;
	private boolean johncena = false;
	public BHL_Money () {
		nf = new DecimalFormat("##.##");
		im = new ImageIcon(new ImageIcon("BHL_Mult.gif").getImage().getScaledInstance(40, 40, Image.SCALE_DEFAULT));
		appFont = new Font("Old English Text MT", Font.PLAIN, 30);
		this.setLayout(new BorderLayout());

		JPanel northPanel = new JPanel();
		northPanel.setLayout(new GridLayout(1,2));
		JPanel middlePanel = new JPanel();
		middlePanel.setLayout(new GridLayout(4,5));
		JPanel southPanel = new JPanel();
		southPanel.setLayout(new FlowLayout(FlowLayout.CENTER));

		JLabel southLabel = new JLabel("Move around to gain money. Click Snoop Dogg to gain multipliers. Goa: Get rich.");
		southLabel.setFont(appFont);
		southPanel.add(southLabel);
		
		cash = new JLabel("Your money: $ " + nf.format(money));
		cash.setFont(appFont);
		multiplier = new JLabel("Your Multiplier: " + mult );
		multiplier.setFont(appFont);
		northPanel.add(multiplier);
		northPanel.add(cash);

		manyPanel = new JPanel[size];
		snoopLabel = new JLabel[size];
		for (int i = 0; i < manyPanel.length; i++) {
			manyPanel[i] = new JPanel();
			manyPanel[i].setLayout(new FlowLayout(FlowLayout.CENTER));
			snoopLabel[i] = new JLabel(im);
			snoopLabel[i].setVisible(false);
			manyPanel[i].add(snoopLabel[i]);
			middlePanel.add(manyPanel[i]);
		}

		add(northPanel, BorderLayout.NORTH);
		add(middlePanel, BorderLayout.CENTER);
		add(southPanel, BorderLayout.SOUTH);
		addMouseMotionListener(this);
		
		
	}
	// MouseMotionListener methods

	public void mouseDragged(MouseEvent me) {}

	public void mouseMoved (MouseEvent me) {
		if (  (currentX != me.getX() || currentY != me.getY()) ) {
			counter++;
			Ucounter++;
			currentX = me.getX();
			currentY = me.getY();
		}
		if (Ucounter % 150 == 0) {
			base += .01;
		}
		if (counter % 10 == 0) {
			money += base * mult;
			cash.setText("Your money: $" + nf.format(money));
			counter = 0;
		}
		if ( Ucounter % 100== 0 && mult < 100 ) {
			if ( !changed) {
				panelNum = (int)(Math.random() * manyPanel.length);
				snoopLabel[panelNum].setVisible(true);
				snoopLabel[panelNum].addMouseListener(this);
				changed = true;
			} else {
				snoopLabel[panelNum].setVisible(false);
				snoopLabel[panelNum].removeMouseListener(this);

				panelNum = (int)(Math.random() * manyPanel.length);
				snoopLabel[panelNum].setVisible(true);
				snoopLabel[panelNum].addMouseListener(this);
			}
		}
		if ( money > 1000 && !johncena) {
			johncena = true;
			playJohnCena();
		}		
	}
	// MouseListener methods
	
	public void mouseClicked(MouseEvent me){
		if (me.getSource() == snoopLabel[panelNum]) {
			mult++;
			multiplier.setText("Your Multiplier: " + mult);
			snoopLabel[panelNum].setVisible(false);
			snoopLabel[panelNum].removeMouseListener(this);
		}
	}
	public void mouseEntered(MouseEvent me){}
	public void mouseExited(MouseEvent me){}
	public void mousePressed(MouseEvent me) {}
	public void mouseReleased(MouseEvent me){}
		
	public void playJohnCena() {
		this.removeAll();
		ImageIcon im = new ImageIcon(new ImageIcon("BHL_Superman.gif").getImage().getScaledInstance(1000, 562, Image.SCALE_DEFAULT));	
		this.add(new JLabel(im));
		this.revalidate();
		
		File cenation = new File("BHL_Trumpet.wav");
    		 try {
       			Clip dclip = AudioSystem.getClip();
			dclip.open(AudioSystem.getAudioInputStream(cenation));
			dclip.start();
    		} catch(Exception ex) {
      			  System.out.println("Error.");
 
 		}
	}
	
}