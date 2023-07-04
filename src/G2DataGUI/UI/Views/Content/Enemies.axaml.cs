using Avalonia.Controls;
using G2DataGUI.UI.ViewModels;

namespace G2DataGUI.UI.Views.Content;

public partial class Enemies : UserControl
{
    public Enemies()
    {
        DataContext = EnemiesViewModel.Instance;
        InitializeComponent();
    }
}