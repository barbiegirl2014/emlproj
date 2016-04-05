/*
'''
table VirusList 病毒黑白名单
id      主键，自动生成
owner   规则拥有者，用户创建的规则，owner记为该用户的邮件地址；
                   域管理员创建的规则，owner为该域域名，适用于该域收发的邮件；
                   网关管理员创建的规则，owner为GLOBAL，适用于全网关收发的邮件
level   规则层级， 0（用户级），1（域级），2（网关级）
field   匹配域
value   匹配值
type    0（黑）、1（白）
direction 0（进）、1（出）


table NetgateDomain 域名列表
id      主键，自动生成
domain  域名
'''

#跟垃圾邮件的逻辑一模一样


#virusCheck(email, sender, 1)
#virusCheck(email, receiver, 0)
def virusCheck(email,owner,direction):
    
    if owner not in netgate:
        return 'None'
        
    #1.user级处理
    if direction == 1: #发出的邮件
        #不处理或只进行黑名单处理
        #下面至今进行黑名单处理
        user_blist = SELECT * FROM VirusList WHERE owner == owner AND level == 0 AND type == 0 AND direction == direction
        user_wlist = empty
        
    else:#收到的邮件
        user_blist = SELECT * FROM VirusList WHERE owner == owner AND level == 0 AND type == 0 AND direction == direction 
        user_wlist = SELECT * FROM VirusList WHERE owner == owner AND level == 0 AND type == 1 AND direction == direction
    
    if email match user_blist:
        return 'Confirmed'
    if email match user_wlist:
        return 'OK'
    
    #2.domain级处理
    domain = getDomain(owner)
    
    domain_blist = SELECT * FROM VirusList WHERE owner == domain AND level == 1 AND type == 0 AND direction == direction
    domain_wlist = SELECT * FROM VirusList WHERE owner == domain AND level == 1 AND type == 1 AND direction == direction
    
    if email match domain_blist:
        return 'Confirmed'
    if email match domain_wlist:
        return 'OK'
    
    #3.网关级处理
    global_blist = SELECT * FROM VirusList WHERE owner == GLOBAL AND level == 2 AND type == 0 AND direction == direction
    global_wlist = SELECT * FROM VirusList WHERE owner == GLOBAL AND level == 2 AND type == 1 AND direction == direction
    
    if email match global_blist:
        return 'Confirmed'
    if email match global_wlist:
        return 'OK'
        
    #4.病毒引擎处理
    result = virusEngineCheck(email)
    if result == 'Spam':
        return 'Confirmed'
    return 'OK'
*/