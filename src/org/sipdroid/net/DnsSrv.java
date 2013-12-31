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

package org.sipdroid.net;
import java.lang.reflect.Method;
import org.zoolu.net.*;

public final class DnsSrv {
	public static DnsSrv GetInstance() {
		return instance;
	}

	public SocketAddress[] getHostSRV(String name, String srv, String proto) {
		String dns1 = "8.8.8.8";
		String dns2 = "8.8.8.4";
		if(!loaded) return null;
		try{
			if(GetSystemPropertie != null) {
				dns1 = (String) GetSystemPropertie.invoke(null, "net.dns1");
				dns2 = (String) GetSystemPropertie.invoke(null, "net.dns2");
			}
		}catch(Exception e)
		{}

		String[] ret = getHostSRV(name, srv, proto, 
		dns1, dns2);
		if(ret != null) {
			SocketAddress[] addrs = new SocketAddress[ret.length];
			for (int i = 0; i < ret.length; i++) {
				String[] entry = ret[i].split("\\s*,\\s*");
				if(entry.length == 4) {
					addrs[i] = new SocketAddress(entry[0], Integer.parseInt(entry[1]));
				} else {
					addrs[i] = null;
				}
			}
			return addrs;
		} else {
			return null;
		}
	}
	
	private DnsSrv() {}
	
	private static DnsSrv instance = new DnsSrv();
	private static Method GetSystemPropertie = null;
	private static boolean loaded = false;
	private native String[] getHostSRV(String name, String srv, String proto, String dns0, String dns1);

	static {
			try {
		        System.loadLibrary("DnsSrv");
				loaded = true;
				if(GetSystemPropertie == null)
					GetSystemPropertie = Class.forName("android.os.SystemProperties").getMethod("get", new Class[] { String.class });	
			} catch (Throwable e) {
			}
	}
}
