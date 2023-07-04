﻿using System;
using System.Collections.ObjectModel;
using System.IO;

namespace G2DataGUI.Common.Data.Moves;

public class Moves
{
    public static Moves Instance { get; } = new();
    private ObservableCollection<Move> _moves = new();
    public event EventHandler CollectionRefreshed;
    public static int NumberOfMoves { get; } = 0x80;

    private Moves()
    {
        ReadMoves();
    }

    public void Save()
    {
        WriteMoves();
    }

    public void Reload()
    {
        ReadMoves();
    }

    private void ReadMoves()
    {
        _moves.Clear();
        using (FileStream reader = File.Open(Version.Instance.RootDataDirectory + GameFilePaths.MovesPath, FileMode.Open, FileAccess.Read))
        using (MemoryStream memReader = new MemoryStream())
        {
            reader.CopyTo(memReader);
            memReader.Seek(0, SeekOrigin.Begin);
            while (memReader.Position < memReader.Length)
            {
                Move move = Move.ReadMove(memReader);
                if (move != null)
                {
                    move.ID = Convert.ToByte(_moves.Count);
                    _moves.Add(move);
                }
            }
        }
        CollectionRefreshed?.Invoke(this, EventArgs.Empty);
    }

    private void WriteMoves()
    {
        using (FileStream writer = File.Open(Version.Instance.RootDataDirectory + GameFilePaths.MovesPath, FileMode.OpenOrCreate, FileAccess.ReadWrite))
        {
            foreach (Move move in _moves)
            {
                move.WriteMove(writer);
            }
        }
    }

    public ObservableCollection<Move> GetMoves() { return _moves; }
}