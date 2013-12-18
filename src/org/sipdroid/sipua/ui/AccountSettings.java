/*
 * Copyright (C) 2009 The Sipdroid Open Source Project
 * 
 * This file is part of Sipdroid (http://www.sipdroid.org)
 * 
 * Sipdroid is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this source code; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package org.sipdroid.sipua.ui;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

import org.sipdroid.codecs.Codecs;
import org.sipdroid.media.RtpStreamReceiver;
import org.sipdroid.sipua.R;
import org.sipdroid.sipua.SipdroidEngine;
import org.zoolu.sip.provider.SipStack;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences.Editor;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.text.InputType;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Toast;

public class AccountSettings extends PreferenceActivity implements OnSharedPreferenceChangeListener, OnClickListener {

	public static final String PREF_LINES = "lines";
	
	private SharedPreferences settings = null;
		
	// IDs of the menu items
	private static final int MENU_PLUS = 0;
	private static final int MENU_MINUS = 1;
	//
	private static final int MAX_LINES = 5;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		settings = PreferenceManager.getDefaultSharedPreferences(Receiver.mContext);
		addPreferencesFromResource(R.xml.account_settings);

		setDefaultValues(MAX_LINES);
		
		UpdatePreferenceScreen();
		settings.registerOnSharedPreferenceChangeListener(this);
	}
	
	void reload() {
		//setPreferenceScreen(null);
		//addPreferencesFromResource(R.xml.account_settings);	
		UpdatePreferenceScreen();		
	}
	
	private void UpdatePreferenceScreen() {
		for (int i = 0; i < MAX_LINES; i++) {
			String j = (i!=0?""+i:"");
			PreferenceScreen account_i = (PreferenceScreen)getPreferenceScreen().findPreference(Settings.PREF_ACCOUNT+j);
			account_i.setEnabled(i < SipdroidEngine.LINES ? true:false);
		}
	}

	private void setDefaultValues(int lines) {
		if (settings.getInt(PREF_LINES, -1) == -1) {
			Editor edit = settings.edit();
			edit.putInt(PREF_LINES, SipdroidEngine.LINES);
			edit.commit();
		}
		
		for (int i = 0; i < lines; i++) {
			String j = (i!=0?""+i:"");
			if (settings.getString(Settings.PREF_SERVER+j, "").equals("")) {
				CheckBoxPreference cb = (CheckBoxPreference) getPreferenceScreen().findPreference(Settings.PREF_WLAN+j);
				cb.setChecked(true);
				Editor edit = settings.edit();

				edit.putString(Settings.PREF_PORT+j, "5061");
				edit.putString(Settings.PREF_SERVER+j, Settings.DEFAULT_SERVER);
				edit.putString(Settings.PREF_PREF+j, Settings.DEFAULT_PREF);				
				edit.putString(Settings.PREF_PROTOCOL+j, Settings.DEFAULT_PROTOCOL);
				edit.commit();
	        	Receiver.engine(this).updateDNS();
	        	reload();
			}
		}
		updateSummaries(lines);
	}

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
	    menu.add(0, MENU_PLUS, 0, "+");
	    menu.add(0, MENU_MINUS, 0, "-");
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item) {
    	switch (item.getItemId()) {
            case MENU_PLUS:
			{
				if(SipdroidEngine.LINES >= MAX_LINES) return false;

				Editor edit = settings.edit();
				edit.putInt(PREF_LINES, SipdroidEngine.LINES+1);
				edit.commit();
							
				reload();
                return true;
			}
                
            case MENU_MINUS:
			{
				if(SipdroidEngine.LINES < 2) return false;
				Editor edit = settings.edit();
				edit.putInt(PREF_LINES, SipdroidEngine.LINES-1);
				edit.commit();
						
				reload();
                return true;
			}
        }

        return false;
    }

	@Override
	public void onDestroy()	{
		super.onDestroy();

		settings.unregisterOnSharedPreferenceChangeListener(this);
	}

	EditText transferText;
	String mKey;

    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
	    if (!Thread.currentThread().getName().equals("main"))
    		return;
		if (key.startsWith(Settings.PREF_PORT) && sharedPreferences.getString(key, Settings.DEFAULT_PORT).equals("0")) {
	   		Editor edit = sharedPreferences.edit();
    		edit.putString(key, Settings.DEFAULT_PORT);
    		edit.commit();

    		transferText = new InstantAutoCompleteTextView(this,null);
			transferText.setInputType(InputType.TYPE_CLASS_NUMBER);
			mKey = key;

			new AlertDialog.Builder(this)
			.setTitle(Receiver.mContext.getString(R.string.settings_port))
			.setView(transferText)
			.setPositiveButton(android.R.string.ok, this)
			.show();
			return;
		} else if (key.startsWith(Settings.PREF_SERVER) || key.startsWith(Settings.PREF_PROTOCOL)) {
    		Editor edit = sharedPreferences.edit();
    		for (int i = 0; i < SipdroidEngine.LINES; i++) {
    			edit.putString(Settings.PREF_DNS+i, Settings.DEFAULT_DNS);
    			String j = (i!=0?""+i:"");
    			if (key.equals(Settings.PREF_SERVER+j)) {
    				ListPreference lp = (ListPreference) getPreferenceScreen().findPreference(Settings.PREF_PROTOCOL+j);
    				lp.setValue(sharedPreferences.getString(Settings.PREF_SERVER+j, Settings.DEFAULT_SERVER).equals(Settings.DEFAULT_SERVER) ? "tcp" : "udp");
    				lp = (ListPreference) getPreferenceScreen().findPreference(Settings.PREF_PORT+j);
    				lp.setValue(sharedPreferences.getString(Settings.PREF_SERVER+j, Settings.DEFAULT_SERVER).equals(Settings.DEFAULT_SERVER) ? "5061" : Settings.DEFAULT_PORT);
    			}
    			if (key.equals(Settings.PREF_PROTOCOL+j)) {
    				if (sharedPreferences.getString(Settings.PREF_SERVER+j, Settings.DEFAULT_SERVER).equals(Settings.DEFAULT_SERVER)) {
    					ListPreference lp = (ListPreference) getPreferenceScreen().findPreference(Settings.PREF_PORT+j);
    					lp.setValue(sharedPreferences.getString(Settings.PREF_PROTOCOL+j, Settings.DEFAULT_PROTOCOL).equals("tls") ? "5070" : "5061");
    				} else {
    		        	Receiver.engine(this).halt();
    		    		Receiver.engine(this).StartEngine();
    				}
    			}
    		}
    		edit.commit();
        	Receiver.engine(this).updateDNS();
        	Checkin.checkin(false);
	    } else if (key.startsWith(Settings.PREF_WLAN) ||
        			key.startsWith(Settings.PREF_3G) ||
        			key.startsWith(Settings.PREF_EDGE) ||
        			key.startsWith(Settings.PREF_USERNAME) ||
        			key.startsWith(Settings.PREF_PASSWORD) ||
        			key.startsWith(Settings.PREF_DOMAIN) ||
        			key.startsWith(Settings.PREF_SERVER) ||
        			key.startsWith(Settings.PREF_PORT) ||
        			key.startsWith(Settings.PREF_PROTOCOL) ||
        			key.startsWith(Settings.PREF_VPN) ||
        			key.startsWith(Settings.PREF_FROMUSER)) {
        	Receiver.engine(this).halt();
    		Receiver.engine(this).StartEngine();
		} else if (key.equals(PREF_LINES)) {
			Receiver.engine(this).halt();
			Receiver.engine(this).UpdateLines();
    		Receiver.engine(this).StartEngine();
		}
		updateSummaries(SipdroidEngine.LINES);
    }

	public void updateSummaries(int lines) {	
    	
       	for (int i = 0; i < lines; i++) {
       		String j = (i!=0?""+i:"");
       		String username = settings.getString(Settings.PREF_USERNAME+j, Settings.DEFAULT_USERNAME),
       			server = settings.getString(Settings.PREF_SERVER+j, Settings.DEFAULT_SERVER);
	    	getPreferenceScreen().findPreference(Settings.PREF_USERNAME+j).setSummary(username); 
	    	getPreferenceScreen().findPreference(Settings.PREF_SERVER+j).setSummary(server);
	    	if (settings.getString(Settings.PREF_DOMAIN+j, Settings.DEFAULT_DOMAIN).length() == 0) {
	    		getPreferenceScreen().findPreference(Settings.PREF_DOMAIN+j).setSummary(getString(R.string.settings_domain2));
	    	} else {
	    		getPreferenceScreen().findPreference(Settings.PREF_DOMAIN+j).setSummary(settings.getString(Settings.PREF_DOMAIN+j, Settings.DEFAULT_DOMAIN));
	    	}
	    	if (settings.getString(Settings.PREF_FROMUSER+j,Settings.DEFAULT_FROMUSER).length() == 0) {
	    		getPreferenceScreen().findPreference(Settings.PREF_FROMUSER+j).setSummary(getString(R.string.settings_callerid2));
	    	} else {
	    		getPreferenceScreen().findPreference(Settings.PREF_FROMUSER+j).setSummary(settings.getString(Settings.PREF_FROMUSER+j, Settings.DEFAULT_FROMUSER));
	    	}
	    	getPreferenceScreen().findPreference(Settings.PREF_PORT+j).setSummary(settings.getString(Settings.PREF_PORT+j, Settings.DEFAULT_PORT));
	    	getPreferenceScreen().findPreference(Settings.PREF_PROTOCOL+j).setSummary(settings.getString(Settings.PREF_PROTOCOL+j,
	    		settings.getString(Settings.PREF_SERVER+j, Settings.DEFAULT_SERVER).equals(Settings.DEFAULT_SERVER) ? "tcp" : "udp").toUpperCase());
	    	getPreferenceScreen().findPreference(Settings.PREF_ACCOUNT+j).setSummary(username.equals("")||server.equals("")?getResources().getString(R.string.settings_line)+" "+(i+1):username+"@"+server);
       	}
   }

    @Override
	public void onClick(DialogInterface arg0, int arg1) {
		Editor edit = settings.edit();
 		edit.putString(mKey, transferText.getText().toString());
		edit.commit();
	}
}