﻿using G2DataGUI.Common.Data.Skills;
using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace G2DataGUI.UI.ViewModels;

public sealed class SkillsViewModel : BaseViewModel
{
    public ObservableCollection<Skill> GameSkills { get; private set; } = Skills.Instance.GetSkills();
    private int _selectedSkillIndex = 0;
    private Skill _selectedSkillItem;

    public static SkillsViewModel Instance { get; private set; } = new();

    private SkillsViewModel()
    {
        _selectedSkillItem = GameSkills.First();
    }

    private void SkillsCollectionRefreshed(object sender, EventArgs _)
    {
        
    }

    public int SelectedSkillIndex
    {
        get => _selectedSkillIndex;
        set
        {
            if (value < 0) value = 0;
            if (value >= GameSkills.Count) return;
            _selectedSkillIndex = value;
            SelectedSkillItem = GameSkills[value];
            OnPropertyChanged(nameof(SelectedSkillIndex));
        }
    }

    public Skill SelectedSkillItem
    {
        get => _selectedSkillItem;
        set
        {
            _selectedSkillItem = value;
            OnPropertyChanged(nameof(SelectedSkillItem));
        }
    }
}