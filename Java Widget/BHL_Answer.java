// bao le
// occc spring 2018
// advanced java
// your name panel

import javax.swing.*;			// control
import java.awt.*;			// back end, listeners, etc 
import java.awt.event.*;		// event handers

import java.io.*;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
public class BHL_Answer extends JPanel implements ActionListener {
	
	JPanel[] listPanel;
	JButton[] listButton;
	JButton[] doneButton;
	JLabel[] listPrompt;
	JLabel daLabel1;
	private int x = 0, counter, num;
	JPanel centerPanel, southPanel;	
	Font appFont;
	
	public BHL_Answer() {
		appFont = new Font("Arial", Font.PLAIN, 25);
		this.setLayout(new BorderLayout());	
		
		JPanel northPanel = new JPanel();
		northPanel.setLayout(new FlowLayout(FlowLayout.CENTER));
		JLabel instruction = new JLabel("Please click the Button as many time as the number shown. Click done when you're done or you need to reset.");
		instruction.setFont(appFont);		
		northPanel.add(instruction);
		
		centerPanel = new JPanel();
		centerPanel.setLayout(new GridLayout(3,0));
		southPanel = new JPanel();

		listPanel = new JPanel[3];
		listButton = new JButton[3];
		listPrompt = new JLabel[3];
		doneButton = new JButton[3];

		nextTest();

		add(centerPanel, BorderLayout.CENTER);	
		add(northPanel, BorderLayout.NORTH);	
		
		// addMouseListener(this);
	}
	public void nextTest() {
		
		num = (int)(Math.random() * 18) + 2;

		listPanel[x] = new JPanel();
		listPanel[x].setLayout(new GridLayout(1,2));

		// 2 panel inside listpanel
		JPanel leftPanel = new JPanel();
		JPanel rightPanel = new JPanel();
		leftPanel.setLayout(new BorderLayout());
		rightPanel.setLayout(new BorderLayout());	

		JPanel leftPanels = new JPanel();
		JPanel rightPanels = new JPanel();
		leftPanels.setLayout(new FlowLayout(FlowLayout.CENTER));
		rightPanels.setLayout(new FlowLayout(FlowLayout.CENTER));	

		listPrompt[x] = new JLabel("" + num + "\t");
		listPrompt[x].setFont(appFont);
		leftPanels.add(listPrompt[x]);		

		listButton[x] = new JButton("Click Me");
		doneButton[x] = new JButton("Done");
		listButton[x].setFont(appFont);
		doneButton[x].setFont(appFont);
		rightPanels.add(listButton[x]);
		rightPanels.add(doneButton[x]);		

		leftPanel.add(leftPanels, BorderLayout.CENTER);
		rightPanel.add(rightPanels, BorderLayout.CENTER);

		listPanel[x].add(leftPanel);
		listPanel[x].add(rightPanel);
		
		centerPanel.add(listPanel[x]);	
		
		doneButton[x].addActionListener(this);
		listButton[x].addActionListener(this);
	}
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == listButton[x]) {
			counter++;
			if ( x==0 || x==2) {
				listButton[x].setText("Counter: " + counter);
			}
			if ( counter== (int)(num/2) && x==2 ) {
				playJohnCena();
			}
		}
		if (e.getSource() == doneButton[x]) {
			if ( counter != num ) {
				counter = 0;
				if ( x==0 ) {
					listButton[x].setText("Counter: " + counter);
				}
			} 

			if ( num==counter && x==0) {
				x++;
				counter = 0;
				daLabel1 = new JLabel("Try without the counter.");
				daLabel1.setFont(appFont);
				southPanel.add(daLabel1);
				add(southPanel, BorderLayout.SOUTH);
				nextTest();
			} else if ( num==counter && x==1 ) {
				x++;
				counter = 0;
				appFont = new Font("Arial", Font.PLAIN, 9);
				daLabel1.setText("Look closer if you need. You get counter again this time :). Also remember this sequence: 314159");
				nextTest();
			}
		}
	}
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