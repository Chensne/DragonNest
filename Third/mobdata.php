<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" />
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" /> 
<meta http-equiv="Content-Language" content="zh-cn" /> 
<meta name="author" content="WindWT" /> 
<meta name="Copyright" content="Copyright WindWT@C0de::W1ndWT All Rights Reserved." />  
<title>魔物属性查看器</title>
</head>
<body>
<form>
<select name='diff'>
<option value='0'>简单</option>
<option value='1'>普通</option>
<option value='2'>困难</option>
<option value='3'>大师</option>
<option value='4' selected="">深渊</option>
</select>
<select name='file'>
<option value='monstertable.ext' selected="">副本</option>
<option value='monstertable_champion.ext'>冠军</option>
<option value='monstertable_darklair.ext'>黑暗边界</option>
<option value='monstertable_event.ext'>活动</option>
<option value='monstertable_nest.ext'>巢穴</option>
<option value='monstertable_quest.ext'>任务</option>
</select>
<select name='grade'>
<option value='1'>绿</option>
<option value='2'>?</option>
<option value='3'>黄</option>
<option value='4'>紫</option>
<option value='5'>BOSS</option>
<option value='6'>巢穴中BOSS</option>
<option value='7' selected="">巢穴最终BOSS</option>
<option value='8'>Dragon</option>
</select>
Lv<input maxlength="3" name='level'/>
<input type="checkbox" name="state"/>显示免疫
<input type="checkbox" name="AI"/>显示AI
<input type="checkbox" name="ItemDropGroupID" disabled=""/>掉落物品ID
<input type="checkbox" name="EXP"/>经验
<input type="checkbox" name="noweight"/>不处理权重
<input type="submit" />
</form>
<?php
/////////////////////////
//$uistring[int NameID]
/////////////////////////
$uixml=simplexml_load_file("uistring.xml");
$uistring=Array();
foreach($uixml->message as $message)
{
    $mid=strval($message->attributes());
    $data=strval($message);
    $uistring[$mid]=$data;
}

if(isset($_REQUEST['diff']))
    $diff=$_REQUEST['diff'];
else
    $diff=4;
if(isset($_REQUEST['file']))
    $mobarray=ext2array($_REQUEST['file']);
else
    die();
$mobwarray=wext2array('monsterweighttable.ext',$diff);
/////////////////////////
//table build
/////////////////////////
echo '<table>';
echo '<tr><td>怪物ID</td><td>怪物名</td><td>LV</td><td>等级</td><td>HP</td><td>最小物攻</td><td>最大物攻</td><td>最小魔攻</td><td>最大魔攻</td><td>物防</td><td>魔防</td><td>硬直</td><td>致命</td><td>眩晕</td>';
echo '<td>硬直抵抗</td><td>致命抵抗</td><td>眩晕抵抗</td><td>属性攻击类型</td><td>属性攻击</td><td>火抗</td><td>水抗</td><td>光抗</td><td>暗抗</td><td>超级盔甲</td>';
if($_REQUEST['state'])
    echo '<td>免疫类型</td><td>免疫概率</td>';
if($_REQUEST['AI'])
    echo '<td>AI文件名</td>';
if($_REQUEST['ItemDropGroupID'])
    echo '<td>掉落物品ID</td>';
if($_REQUEST['EXP'])
    echo '<td>经验</td><td>通关经验</td>'; //未实装
echo '</tr>';
foreach($mobarray as $mobdata)
{
    $outputstring='<tr>';
    $outputstring.='<td>';
    $outputstring.=$mobdata[0]; //MonsterID
    $outputstring.='</td>';
    $outputstring.='<td>';
    switch($mobdata[1])
    {
        case 1000003651:
        {
            $mobnameID=$mobdata[2];
            $mobnameID=str_replace('{','',$mobnameID);
            $mobnameID=str_replace('}','',$mobnameID);
            $mobnameID=str_replace('"','',$mobnameID);
            $mobname=$uistring[$mobnameID];
            break;
        }
        case 1000004083:
        {
            $mobnameID=$mobdata[2];
            $mobnameID=str_replace('{','',$mobnameID);
            $mobnameID=str_replace('}','',$mobnameID);
            $mobnameID=str_replace('"','',$mobnameID);
            $mobnameIDArray=explode(',',$mobnameID);
            $mobname=$uistring[$mobnameIDArray[0]].$uistring[$mobnameIDArray[1]];
            break;
        }
        case 0:
        {
            $mobname='[NULL]';
            break;
        }
        default:
        {
            $mobname=$uistring[$mobdata[1]];
            break;
        }
    }
    $outputstring.=$mobname;
    $outputstring.='</td>';
    $outputstring.='<td>';
    if($_REQUEST['level'])
        if($_REQUEST['level']!=$mobdata[3])
            continue;
    else;
    $outputstring.=$mobdata[3];
    $outputstring.='</td>';
    $outputstring.='<td>';
    if($_REQUEST['grade']!=$mobdata[58])
        continue;
    $outputstring.=$mobdata[58];
    $outputstring.='</td>';
    for($i=4;$i<=16;$i++)
    {
        $outputstring.='<td>';
        if($_REQUEST['noweight'])
            $outputstring.=round($mobdata[$i]);
        else
            $outputstring.=round($mobdata[$i]*$mobwarray[$mobdata[0]][$i+2]);
        $outputstring.='</td>';
    }
    $outputstring.='<td>';
    $outputstring.=$mobdata[17];
    $outputstring.='</td>';
    for($i=18;$i<=22;$i++)
    {
        $outputstring.='<td>';
        if($_REQUEST['noweight'])
            $outputstring.=round($mobdata[$i],2);
        else
            $outputstring.=round($mobdata[$i]*$mobwarray[$mobdata[0]][$i+2],2);
        $outputstring.='</td>';
    }
    $outputstring.='<td>';
    if($_REQUEST['noweight'])
        $outputstring.=$mobdata[47];
    else
        $outputstring.=$mobdata[47]*$mobwarray[$mobdata[0]][19];
    $outputstring.='</td>';
    if($_REQUEST['state'])
    {
        $outputstring.='<td>';
        $outputstring.=$mobdata[48];
        $outputstring.='</td>';
        $outputstring.='<td>';
        $outputstring.=$mobdata[49];
        $outputstring.='</td>';
    }
    if($_REQUEST['AI'])
    {
        $outputstring.='<td>';
        $outputstring.=$mobdata[31];
        $outputstring.='</td>';
    }
    if($_REQUEST['ItemDropGroupID'])
    {
        $outputstring.='<td>';
        $outputstring.=$mobdata[61];
        $outputstring.='</td>';
    }
    if($_REQUEST['EXP'])
    {
        $outputstring.='<td>';
        if(!$_REQUEST['noweight'])
            $outputstring.=$mobdata[23];
        else
            $outputstring.=$mobdata[23]*$mobwarray[$mobdata[0]][4];
        $outputstring.='</td>';
        $outputstring.='<td>';
        if(!$_REQUEST['noweight'])
            $outputstring.=$mobdata[24];
        else
            $outputstring.=$mobdata[24]*$mobwarray[$mobdata[0]][5];
        $outputstring.='</td>';
    }
    $outputstring.='</tr>';
    echo $outputstring;
    set_time_limit(60);
}
echo '</table>';
/////////////////////////
function ext2array($filename)
{
	$file=fopen($filename,"rb");
	$extcode_array=unpack(l,fread($file,4));
	$column_array=unpack(l,fread($file,4));
	$line_array=unpack(l,fread($file,4));
	$extcode=$extcode_array[1];
	$column=$column_array[1];
	$line=$line_array[1];
	$type[0]=3;
	for($i=1;$i<=$column;$i++)
	{
		$columntitle_array=unpack('a*',fread($file,64));
		$columntype_array=unpack(l,fread($file,4));
		$type[$i]=$columntype_array[1];
		$tempstring.=$columntitle_array[1]."[$type[$i]]"."\t";
	}
    $tempstring=Array();
	for($j=0;$j<$line;$j++)
	{
		for($i=0;$i<=$column;$i++)
		{
			switch($type[$i])
			{
				case 1:
				{
					$stringlength_array=unpack(v,fread($file,2));
					if($stringlength_array[1]>0)
						$string='"'.iconv("euc-kr","UTF-8",fread($file,$stringlength_array[1])).'"';
					else
						$string='"[NULL]"';
					$tempstring[$j][$i]=$string;
					break;
				}
				case 2:
				{
					$unsignedlong_array=unpack(V,fread($file,4));
					$tempstring[$j][$i]=$unsignedlong_array[1];
					break;
				}
				case 3:
				{
					$signedlong_array=unpack(l,fread($file,4));
					$tempstring[$j][$i]=$signedlong_array[1];
					break;
				}
				case 4:
				{
					$floatlong_array=unpack(f,fread($file,4));
					$tempstring[$j][$i]=$floatlong_array[1];
					break;
				}
				case 5:
				{
					$floatlong_array=unpack(f,fread($file,4));
					$tempstring[$j][$i]=$floatlong_array[1];
					break;
				}
				default:
				{
					$WTF_array=unpack(i,fread($file,4));
					$tempstring[$j][$i]=$WTF_array[1].'(WTF)';
					break;
				}
			}
		}
		set_time_limit(60);
	}
    fclose($file);
	return $tempstring;
}
function wext2array($filename,$diff)
{
	$file=fopen($filename,"rb");
	$extcode_array=unpack(l,fread($file,4));
	$column_array=unpack(l,fread($file,4));
	$line_array=unpack(l,fread($file,4));
	$extcode=$extcode_array[1];
	$column=$column_array[1];
	$line=$line_array[1];
	$type[0]=3;
	for($i=1;$i<=$column;$i++)
	{
		$columntitle_array=unpack('a*',fread($file,64));
		$columntype_array=unpack(l,fread($file,4));
		$type[$i]=$columntype_array[1];
		$tempstring.=$columntitle_array[1]."[$type[$i]]"."\t";
	}
    $tempstring=Array();
    $tempmobwarray=Array();
	for($j=0;$j<$line;$j++)
	{
        /*$signedlong_array=unpack(l,fread($file,4));
        $signedlong_array=unpack(l,fread($file,4));
		$mobID=$signedlong_array[1];
        $tempstring[$mobID][1]=$mobID;
        $signedlong_array=unpack(l,fread($file,4));
        $tempstring[$mobID][2]=$signedlong_array[1];
        if($tempstring[$mobID][2]!=$diff)
        {
            unset($tempstring[$mobID]);
            continue;
        }*/
		for($i=0;$i<=$column;$i++)
		{
			switch($type[$i])
			{
				case 1:
				{
					$stringlength_array=unpack(v,fread($file,2));
					if($stringlength_array[1]>0)
						$string='"'.iconv("euc-kr","UTF-8",fread($file,$stringlength_array[1])).'"';
					else
						$string='"[NULL]"';
					$tempstring[$i]=$string;
					break;
				}
				case 2:
				{
					$unsignedlong_array=unpack(V,fread($file,4));
					$tempstring[$i]=$unsignedlong_array[1];
					break;
				}
				case 3:
				{
					$signedlong_array=unpack(l,fread($file,4));
					$tempstring[$i]=$signedlong_array[1];
					break;
				}
				case 4:
				{
					$floatlong_array=unpack(f,fread($file,4));
					$tempstring[$i]=$floatlong_array[1];
					break;
				}
				case 5:
				{
					$floatlong_array=unpack(f,fread($file,4));
					$tempstring[$i]=$floatlong_array[1];
					break;
				}
				default:
				{
					$WTF_array=unpack(i,fread($file,4));
					$tempstring[$i]=$WTF_array[1].'(WTF)';
					break;
				}
			}
		}
        if($tempstring[2]!=$diff)
        {
            unset($tempstring);
        }
        else
            $tempmobwarray[$tempstring[1]]=$tempstring;
		set_time_limit(60);
	}
    fclose($file);
	return $tempmobwarray;
}
?>
</body>