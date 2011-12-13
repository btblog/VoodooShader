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
using System.IO;
using System.Text;
using System.Web;
using System.Net;

namespace VoodooSharp
{
    public class ChangeSetHandler
    {
        public static bool Update(PackageManifest pm, String sfrom, String sto)
        {
            if (pm == null || sto == null || (sfrom == null && sto == null))
            {
                return false;
            }

            Console.WriteLine("Beginning update procedure.");
            Console.WriteLine("  Package: {0}", pm.Package.Name);

            List<Version> versions = new List<Version>(pm.Versions);
            Version v_from = sfrom == null ? null : versions.Find(v => v.Id == sfrom);
            Version v_to   = sto   == null ? null : versions.Find(v => v.Id == sto  );

            if (v_from == null && v_to == null)
            {
                return false;
            }

            Stack<Version> workingSet = new Stack<Version>();
            Version check = v_to == null ? v_from : v_to;

            while (check != null && check.Id != sfrom)
            {
                workingSet.Push(check);
                check = versions.Find(v => v.Id == check.Parent);
            }

            if (v_to == null)
            {
                VoodooRegistry.Instance.RemovePackage(pm.Package.PackId);
            } else if (workingSet.Count == 0)
            {
                return false;
            }

            Package pack = new Package();
            pack.HomeUri = pm.Package.HomeUri;
            pack.Name = pm.Package.Name;
            pack.PackId = pm.Package.PackId;
            pack.Version = sto;

            Console.WriteLine("{0} updates to be applied.", workingSet.Count);
            while (workingSet.Count > 0)
            {
                Version wv = workingSet.Pop();
                pack.Version = wv.Id;

                if (v_to == null)
                {
                    if (!UninstallVersion(wv))
                    {
                        VoodooRegistry.Instance.SetPackage(pack);
                        return false;
                    }
                }
                else
                {
                    if (!InstallVersion(pm.PackageUri, wv))
                    {
                        VoodooRegistry.Instance.SetPackage(pack);
                        return false;
                    }
                }
            }

            if (v_to == null)
            {
                VoodooRegistry.Instance.RemovePackage(pm.Package.PackId);
            }
            else
            {
                VoodooRegistry.Instance.SetPackage(pack);
            }

            VoodooRegistry.Instance.Write();

            return true;
        }

        public static bool InstallVersion(String root, Version mv)
        {
            VoodooRegistry.Instance.Write();
            VoodooRegistry.Instance.Update();

            if (mv == null) return false;

            Console.WriteLine("Installing version: " + mv.Id);

            if (mv.Remove != null)
            {
                if (mv.Remove.File != null)
                {
                    foreach (String basename in mv.Remove.File)
                    {
                        try
                        {
                            String fullpath = Path.GetFullPath(Path.Combine(VoodooRegistry.Instance.Path, basename));
                            if (!fullpath.StartsWith(VoodooRegistry.Instance.Path))
                            {
                                Console.WriteLine("Illegal file path, aborting version change.", fullpath);
                                Console.WriteLine("This may be a security risk, please contact the package developers and notify the Voodoo Shader developers.");
                                Console.WriteLine("  File: {0}", basename);
                                return false;
                            }
                            File.Delete(fullpath);
                        }
                        catch (Exception exc)
                        {
                            Console.WriteLine("Error removing file. This file may need to be removed manually.");
                            Console.WriteLine("  File: {0}", basename);
                            Console.WriteLine("  Error: {0}", exc.Message);
                        }
                    }
                }

                if (mv.Remove.Default != null)
                {
                    foreach (Default def in mv.Remove.Default)
                    {
                        Console.WriteLine("Removing Default: {0}", def.Name);
                        if (VoodooRegistry.Instance.RemoveDefault(def.DefId) == 0)
                        {
                            Console.WriteLine("  Default not found for removal.", def.DefId);
                        }
                    }
                }

                if (mv.Remove.Module != null)
                {
                    foreach (Module mod in mv.Remove.Module)
                    {
                        Console.WriteLine("Removing Module: {0}", mod.Name);
                        if (VoodooRegistry.Instance.RemoveModule(mod.LibId) == 0)
                        {
                            Console.WriteLine("  Module not found for removal.", mod.LibId);
                        }
                    }
                }
            }

            if (mv.Create != null)
            {
                if (mv.Create.File != null)
                {
                    WebClient wc = new WebClient();
                    wc.DownloadProgressChanged += new DownloadProgressChangedEventHandler(wc_DownloadProgressChanged);
                    foreach (String basename in mv.Create.File)
                    {
                        try
                        {
                            String localpath = Path.GetFullPath(Path.Combine(VoodooRegistry.Instance.Path, basename));
                            if (!localpath.StartsWith(VoodooRegistry.Instance.Path))
                            {
                                Console.WriteLine("Illegal file path, aborting version change.", localpath);
                                Console.WriteLine("This may be a security risk, please contact the package developers and notify the Voodoo Shader developers.");
                                Console.WriteLine("  File: {0}", basename);
                                return false;
                            }

                            String source = root + "/" + mv.VersionUri + "/" + basename;
                            Console.WriteLine("Downloading: {0}", localpath);
                            wc.DownloadFile(source.ToString(), localpath);
                            Console.WriteLine(" done.");
                        }
                        catch (Exception exc)
                        {
                            Console.WriteLine("Error removing file. This file may need to be removed manually.");
                            Console.WriteLine("  File: {0}", basename);
                            Console.WriteLine("  Error: {0}", exc.Message);
                        }
                    }
                }

                if (mv.Create.Default != null)
                {
                    foreach (Default def in mv.Create.Default)
                    {
                        Console.WriteLine("Creating Default: {0}", def.Name);
                        VoodooRegistry.Instance.SetDefault(def);
                    }
                }

                if (mv.Create.Module != null)
                {
                    foreach (Module mod in mv.Create.Module)
                    {
                        Console.WriteLine("Creating Module: {0}", mod.Name);
                        VoodooRegistry.Instance.SetModule(mod);
                    }
                }
            }

            VoodooRegistry.Instance.Write();

            return true;
        }

        public static bool UninstallVersion(Version mv)
        {
            VoodooRegistry.Instance.Write();
            VoodooRegistry.Instance.Update();

            if (mv == null) return false;

            Console.WriteLine("Installing version: " + mv.Id);

            if (mv.Create != null)
            {
                if (mv.Create.File != null)
                {
                    foreach (String basename in mv.Create.File)
                    {
                        try
                        {
                            String fullpath = Path.GetFullPath(Path.Combine(VoodooRegistry.Instance.Path, basename));
                            if (!fullpath.StartsWith(VoodooRegistry.Instance.Path))
                            {
                                Console.WriteLine("Illegal file path, aborting version change.", fullpath);
                                Console.WriteLine("This may be a security risk, please contact the package developers and notify the Voodoo Shader developers.");
                                Console.WriteLine("  File: {0}", basename);
                                return false;
                            }
                            File.Delete(fullpath);
                        }
                        catch (Exception exc)
                        {
                            Console.WriteLine("Error removing file. This file may need to be removed manually.");
                            Console.WriteLine("  File: {0}", basename);
                            Console.WriteLine("  Error: {0}", exc.Message);
                        }
                    }
                }

                if (mv.Create.Default != null)
                {
                    foreach (Default def in mv.Remove.Default)
                    {
                        Console.WriteLine("Removing Default: {0}", def.Name);
                        if (VoodooRegistry.Instance.RemoveDefault(def.DefId) == 0)
                        {
                            Console.WriteLine("  Default not found for removal.", def.DefId);
                        }
                    }
                }

                if (mv.Create.Module != null)
                {
                    foreach (Module mod in mv.Remove.Module)
                    {
                        Console.WriteLine("Removing Module: {0}", mod.Name);
                        if (VoodooRegistry.Instance.RemoveModule(mod.LibId) == 0)
                        {
                            Console.WriteLine("  Module not found for removal.", mod.LibId);
                        }
                    }
                }
            }

            VoodooRegistry.Instance.Write();

            return true;
        }

        static void wc_DownloadProgressChanged(object sender, DownloadProgressChangedEventArgs e)
        {
            if (e.ProgressPercentage % 10 == 0)
            {
                Console.Write(".");
            }
        }
    }
}
