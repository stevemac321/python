Attribute VB_Name = "NewMacros"
Sub ReplaceHashes()
Attribute ReplaceHashes.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.ReplaceHashes"
'
' ReplaceHashes Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find.Replacement.Font
        .Superscript = True
        .Subscript = False
    End With
    With Selection.Find
        .Text = "\#([0-9]{1,2})"
        .Replacement.Text = "\1"
        .Forward = True
        .Wrap = wdFindContinue
        .Format = True
        .MatchCase = False
        .MatchWholeWord = False
        .MatchAllWordForms = False
        .MatchSoundsLike = False
        .MatchWildcards = True
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
Sub ChapterHeadings1()
Attribute ChapterHeadings1.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.ChapterHeadings1"
'
' ChapterHeadings1 Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Style = ActiveDocument.Styles("Heading 1")
    With Selection.Find
        .Text = "Chapter"
        .Replacement.Text = "Chapter"
        .Forward = True
        .Wrap = wdFindContinue
        .Format = True
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
Sub RestorePsalms()
Attribute RestorePsalms.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.RestorePsalms"
'
' RestorePsalms Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find
        .Text = "Psalm "
        .Replacement.Text = "Psalms "
        .Forward = True
        .Wrap = wdFindContinue
        .Format = False
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    Selection.TypeText Text:=" "
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find
        .Text = "Psalm "
        .Replacement.Text = "Psalms "
        .Forward = True
        .Wrap = wdFindContinue
        .Format = False
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
Sub DeleteStrayHashes()
Attribute DeleteStrayHashes.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.DeleteStrayHashes"
'
' DeleteStrayHashes Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find
        .Text = "#"
        .Replacement.Text = ""
        .Forward = True
        .Wrap = wdFindContinue
        .Format = False
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
Sub LBCTitle()
Attribute LBCTitle.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.LBCTitle"
'
' LBCTitle Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Style = ActiveDocument.Styles("Title")
    With Selection.Find
        .Text = "London Baptist"
        .Replacement.Text = "London Baptist"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute
    With Selection
        If .Find.Forward = True Then
            .Collapse Direction:=wdCollapseStart
        Else
            .Collapse Direction:=wdCollapseEnd
        End If
        .Find.Execute Replace:=wdReplaceOne
        If .Find.Forward = True Then
            .Collapse Direction:=wdCollapseEnd
        Else
            .Collapse Direction:=wdCollapseStart
        End If
        .Find.Execute
    End With
End Sub
Sub RemoveExtraLines()
Attribute RemoveExtraLines.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.RemoveExtraLines"
'
' RemoveExtraLines Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find
        .Text = "^p^p"
        .Replacement.Text = "^p"
        .Forward = True
        .Wrap = wdFindContinue
        .Format = False
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
Sub Hashes()
Attribute Hashes.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.Hashes"
'
' Hashes Macro
'
'
    Selection.TypeText Text:="#"
End Sub
Sub ParagraphBoldItalic()
Attribute ParagraphBoldItalic.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.ParagraphBoldItalic"
'
' ParagraphBoldItalic Macro
'
'
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find.Replacement.Font
        .Bold = True
        .Italic = True
    End With
    With Selection.Find
        .Text = "Paragraph ([0-9]{1,2})"
        .Replacement.Text = "Paragraph \1"
        .Forward = True
        .Wrap = wdFindContinue
        .Format = True
        .MatchCase = False
        .MatchWholeWord = False
        .MatchAllWordForms = False
        .MatchSoundsLike = False
        .MatchWildcards = True
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
