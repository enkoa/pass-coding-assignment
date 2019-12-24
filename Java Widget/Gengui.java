// me

import java.awt.*;
import javax.swing.*;
public class Gengui extends JFrame {
	public static final int width = 600, height = 600;
	
	public static void main (String[] args) {
		Gengui gg = new Gengui();
		gg.setVisible(true);
	}
	public Gengui () {
		setSize(width, height);
		setTitle("Da Frame");
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		
		Container contentPane = getContentPane();
		contentPane.setLayout(new GridLayout(2,2));
		// contentPane.setLayout(new BorderLayout());
		JPanel jp1 = new BHL_Fun();
		JPanel jp2 = new BHL_ClickIt();
		JPanel jp3 = new BHL_Answer();
		JPanel jp4 = new BHL_Money();
		
		
		this.add(jp2);
		this.add(jp1);
		this.add(jp3);
		this.add(jp4);
	}
}