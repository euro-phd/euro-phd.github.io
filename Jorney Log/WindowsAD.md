# Windows Active Directory 
## Short disclaimer
This post isn't a complete list of Windows AD exploitation techniques. Content here is just a combination of the blog posts available online, documentation from Microsoft, and things observed in a lab environment. Content here is provided **for educational purposes ONLY**.
## Good old classic
### LLMNR abuse - What's your address, again?
Normies use DNS to resolve domain names and corresponding IP addresses. But people at Microsoft came up with a genius idea to implement their own local solution, just in case when DNS is taking too long to respond or doesn't have a record at all. Link-local Multicast Name Resolution ([LLMNR](https://datatracker.ietf.org/doc/rfc4795/)) allows anyone to respond to name solution requests, and if we place ourselves in the spot where responding to query is possible, really bad things may happen. So here we are, exploiting LLMNR poisoning and getting user hashes, as they think that our server is the legitimate server they are looking for.
### SMB signing - Why fix something that's working?
Network shares are really useful when it comes to collaborative work, sharing data across the organisation, and storing sensitive data (please, don't do this and use a proper solution for storing your passwords, I'm tired of xlsx files with admin passwords inside). When it comes to security, the main issue here is signing, which allows the server to validate a client that is accessing the data. If the signing is not enforced (turned off or optional) we can relay hashes from user\machine to SMB share of our interest and get access as we were that particular user.
### AS-REP roasting - REALLY?
I don't even want to talk about that
### Kerberoasting - Don't configure service accounts if you don't know how to do it properly
The nice design feature is exploited by attackers, who would have thought? In reality, it may be an issue if the Kerberoastable accounts aren't managed by AD and their password is not complex enough. By default, AD has a nice password policy for krbtgt and other services (though you may use [gMSA](https://learn.microsoft.com/en-us/windows-server/security/group-managed-service-accounts/getting-started-with-group-managed-service-accounts) or other solution). But when an administrator wants to implement some "nice feature" and decides that it's a good idea to use user accounts for service it all becomes a mess.
### Other relay techniques - Try them all! 
I mentioned SMB relay, next will come LDAP relay, but in reality there are **MUCH, MUCH MORE** types. If you are interested, check out this post on [nltmrelayx](https://www.thehacker.recipes/ad/movement/ntlm/relay)
## Some newer stuff
### Coerse - Come to me
You can't ask a user to connect to your SMB share for the relay, but you can ask a PC (or even better - a DC!) to do this. Coerse techniques allow authenticated attackers (domain user is usually enough, but on unpatched systems, you can even try unauthenticated) to get the machine account hash and relay it somewhere else for exploitation. On one test, I got it working like that: course on DC1 -> relay to LDAP on DC2 -> add a computer with RBCD -> DA credentials.
> [My testing](https://x.com/wa1tf0r_me/status/1719707494841270305) shows that the course from Server 2019 to Server 2019 may not work, while Server 2016 is much more permissive and accepts coerced connections. However, I didn't actually discover why.

### LDAP signing and channel binding - So you want me to add a new computer
For signing we have the same story as for SMB, but channel binding is worth explaining at least a little bit. In Windows AD you often come across LDAP services (or even LDAPS), also you know that [ldapdomaindump](https://github.com/dirkjanm/ldapdomaindump) can extract a lot of useful data. When it comes to security and LDAPS is not an option (let's say you just don't like the idea of moving from port 389 to 636) you can use an LDAP connection to later upgrade to secure connection using TLS/SSL, and if channel binding is not enforced, an attacker can abuse that.
> You can get more info here [TRIMARC post](https://www.hub.trimarcsecurity.com/post/ldap-channel-binding-and-signing) or follow links below.

### AD CS - Certificate Service or Compromentation Service?
For some reason, admins like to configure things and forget about it forever. If your admin is just like that, you may be lucky to exploit one of 13 (at the time of writing) ESC vulnerabilities in Active Directory Certificate Services. Most of the time, you can use [certipy](https://github.com/ly4k/Certipy). 
> However, there's more to it than meets the eye. AD CS can be used to get [**VALID AND TRUSTED CERTIFICATE** for web service](https://www.sygnia.co/blog/guarding-the-bridge-new-attack-vectors-in-azure-ad-connect/), let's say `*.onmicrosoft.com` and if you are lucky to get yourself in MitM position, you can decrypt all traffic going to this domain. I just want to remind you that there is also [DNS records spoofing](https://www.akamai.com/blog/security-research/spoofing-dns-by-abusing-dhcp) present in AD, so you can imagine an attacker with user credential spoofing DNS record to `*.onmicrosoft.com`, using trusted certificate and decrypting traffic from Azure AD Connect host, or even worse - decrypt traffic from user machines to corporate sites.   
### Writeble SMB shares - Slinky and the family 
You found yourself in a situation of disbelief, with no low-hanging fruits in the environment and the only thing that stands out is a popular writable share? If so, you can exploit write permissions to get user hashes. Simply use the crackmapexec/netexec module called `slinky` and whenever a user visits the folder with payload - you will get a hash to relay or crack.
> This is often the last resort in the engagement, and if you ever will use it, please make a file/link/etc. look legitimate. More about the family of slinky-like files in the post from [Red Team Notes](https://www.ired.team/offensive-security/initial-access/t1187-forced-authentication), it's great resource to learn from
## Conclusions
- Windows Active Directory contains a lot of different features and services that may or may not be secure. 
- Don't be afraid to combine different known techniques. If I hadn't combined PetitPotam and LDAP relay on some of my engagements, I wouldn't have a DA in the environment.
- Follow researchers, support researchers, and become a researcher yourself 

## Notes
- Default **Machine Account Quota** in AD is 10 machines, so if you need a machine account you can easily create one using **impacket**
- To bypass password rotation on the first login for a newly created user you can simply change the password immediately after creation, even in the same cmd shell
- SMB signing is off or **not enforced** in MANY environments
- The **MSOL_** account has DCSync rights and credentials are stored in the SQL database on the Azure AD Connect host (you can find the right host based on the cloud-only account that looks like `Sync_SRV01_4bc4a34e95fa@company.onmicrosoft.com`. Here `SRV01` is synchronization host)
## Links to read more
- [Red Team Notes](https://www.ired.team/)
- [Orange Cyberdefense](https://orange-cyberdefense.github.io/ocd-mindmaps/)
- [XPN's InfoSec Blog](https://blog.xpnsec.com/kerberos-attacks-part-1/)
- [Active Directory Security](https://adsecurity.org/?page_id=4031)
- [Hacktricks](https://book.hacktricks.xyz/welcome/readme)
- [Akamai blog](https://www.akamai.com/blog/security-research/spoofing-dns-by-abusing-dhcp)
