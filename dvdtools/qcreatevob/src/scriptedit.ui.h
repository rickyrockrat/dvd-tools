void scriptedit::main(const QString &script)
{
scriptEditText->setText(script);
}

QString scriptedit::becomeStr()
{
return QString(scriptEditText->text());
}
