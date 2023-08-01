﻿using System.Collections.Generic;
using System.IO;
using G2DataGUI.Common.Data.Common;
using G2DataGUI.IO.Streams;

namespace G2DataGUI.Common.Data.Models;

public class NJSVertexList
{
	/// <summary>
	/// Always 0x2900?
	/// </summary>
	public short Format { get; set; }

	/// <summary>
	/// No idea
	/// </summary>
	public short Unknown1 { get; set; }

	/// <summary>
	/// Always 0x0000?
	/// </summary>
	public short Flags { get; set; }

	/// <summary>
	/// Number of Vertices
	/// </summary>
	public short Count { get; set; }

	/// <summary>
	/// Should end with a 0xFF000000?
	/// </summary>
	public List<Vector3> VertexList { get; set; } = new List<Vector3>();

	public static NJSVertexList ReadNJSVertexList(Stream reader)
	{
		NJSVertexList list = new()
		{
			Format = reader.ReadRawShort(),
			Unknown1 = reader.ReadRawShort(),
			Flags = reader.ReadRawShort(),
			Count = reader.ReadRawShort(),
		};

		for (int index = 0; index < list.Count; index++)
		{
			list.VertexList.Add(Vector3.ReadVector3(reader));
		}

		return list;
	}
}
