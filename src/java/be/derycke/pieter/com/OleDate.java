package be.derycke.pieter.com;

import java.util.Date;

public class OleDate extends Date {

	private static final long serialVersionUID = 6901412906760265708L;
	
	public OleDate(Date date) {
		super(date.getTime());
	}
	
    public OleDate(double date) {
       setDate(date);
    }

    public void set(int year, int month, int day, int hours, int minutes, int seconds) {
    	setYear(year);
    	setMonth(month);
    	setDate(day);
    	setHours(hours);
    	setMinutes(minutes);
    	setSeconds(seconds);
    }

    public int get(int index) {
    	switch (index) {
    		case 0: return getYear();
    		case 1: return getMonth();
    		case 2: return getDate();
    		case 3: return getHours();
    		case 4: return getMinutes();
    		case 5: return getSeconds();
    		default: throw new IndexOutOfBoundsException();
    	}
    }
    
    public native void setDate(double value);
	public native double toDouble();
}
