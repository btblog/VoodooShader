﻿/*
 * This file is part of the Voodoo Shader Framework.
 *
 * Copyright (c) 2010-2011 by Sean Sube
 *
 * The Voodoo Shader Framework is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this program; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 US
 *
 * Support and more information may be found at
 *   http://www.voodooshader.com
 * or by contacting the lead developer at
 *   peachykeen@voodooshader.com
 */
using System;
using System.Collections.Generic;
using System.Xml.Serialization;

namespace VoodooSharp
{
    [System.SerializableAttribute()]
    [System.Xml.Serialization.XmlRootAttribute("VoodooPackage", Namespace = "", IsNullable = false)]
    public partial class PackageManifest
    {
        public Package Package { get; set; }
        public string Description { get; set; }
        public string PackageUri { get; set; }

        [System.Xml.Serialization.XmlArrayItemAttribute("Language", IsNullable = false)]
        public List<MessageSet> Messages { get; set; }

        [System.Xml.Serialization.XmlArrayAttribute("Versions", IsNullable = false)]
        [System.Xml.Serialization.XmlArrayItemAttribute("Version", IsNullable = false)]
        public List<PackageVersion> Versions { get; set; }

        public bool Update(String id_to)
        {
            Package installedPack = GlobalRegistry.Instance.GetPackage(Package.PackId);
            String id_from = installedPack == null ? null : installedPack.Version;

            if (id_to == null || (id_from == null && id_to == null))
            {
                return false;
            }

            Console.WriteLine("Beginning update procedure.");
            Console.WriteLine("  Package: {0}", Package.Name);

            PackageVersion v_from = id_from == null ? null : Versions.Find(v => v.Id == id_from);
            PackageVersion v_to = id_to == null ? null : Versions.Find(v => v.Id == id_to);

            if (v_from == null && v_to == null)
            {
                return false;
            }

            Stack<PackageVersion> workingSet = new Stack<PackageVersion>();
            PackageVersion check = v_to == null ? v_from : v_to;

            while (check != null && check.Id != id_from)
            {
                workingSet.Push(check);
                check = Versions.Find(v => v.Id == check.Prev);
            }

            if (v_to == null)
            {
                GlobalRegistry.Instance.RemovePackage(Package.PackId);
            }
            else if (workingSet.Count == 0)
            {
                return false;
            }

            Package pack = new Package();
            pack.HomeUri = Package.HomeUri;
            pack.Name = Package.Name;
            pack.PackId = Package.PackId;
            pack.Version = id_to;

            Console.WriteLine("{0} updates to be applied.", workingSet.Count);
            while (workingSet.Count > 0)
            {
                PackageVersion wv = workingSet.Pop();
                pack.Version = wv.Id;

                if (v_to == null)
                {
                    if (!wv.Uninstall())
                    {
                        GlobalRegistry.Instance.SetPackage(pack);
                        return false;
                    }
                }
                else
                {
                    if (!wv.Install(PackageUri))
                    {
                        GlobalRegistry.Instance.SetPackage(pack);
                        return false;
                    }
                }
            }

            if (v_to == null)
            {
                GlobalRegistry.Instance.RemovePackage(Package.PackId);
            }
            else
            {
                GlobalRegistry.Instance.SetPackage(pack);
            }

            GlobalRegistry.Instance.Write();

            return true;
        }
    }
}
