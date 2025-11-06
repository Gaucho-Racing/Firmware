# Fire_Name_for_Parser_Here

parser description here

**Perl and Yaml are required for compilation, read installation instructions below** 
---
## Installing Perl
### for macOS:
macOS comes with Perl, but you can install a newer version via [HomeBrew](https://brew.sh/).
```
brew install perl
```
### for Linux:
Ubuntu/Debian:
```
sudo apt-get install -y perl build-essential libyaml-dev
```
### for Windows:
Download and install [StrawberryPerl](https://strawberryperl.com/).

## Installing CPANM (for YAML)
```
curl -L https://cpanmin.us | perl - App::cpanminus
```
Ensure Perl's in directory is in your `PATH` with `cpanm --version`. If the command is not found, move it into your `PATH` with the commands below:
#### macOS/Linux:
```
echo 'export PATH="$HOME/perl5/bin:$PATH"' >> ~/.bash_profile
source ~/.bash_profile
```

## Installing YAML::XS
```
cpanm YAML::XS
```
Verify with:
```
perl -MYAML::XS -e 'print "YAML::XS installed\n";'
```




