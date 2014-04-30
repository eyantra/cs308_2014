package gui;

import java.awt.*;
import java.awt.image.BufferedImage;

public class VideoPanel extends Panel
{
	private static final long serialVersionUID = 1L;
	
	private BufferedImage frame;

	@Override
	public void paint(Graphics g) {
		Graphics2D g2d = (Graphics2D) g;			
		g2d.drawImage(frame, 0, 0, this);
		g2d.dispose();
	}
	
	public void updateFrame(BufferedImage frame) {
		this.frame = frame;
		repaint();
	}
}
