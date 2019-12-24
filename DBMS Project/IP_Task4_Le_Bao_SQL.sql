

--delete tables

drop table department_track;
drop table process_track;
drop table assembly_track;
drop table updates;
drop table record;
drop table transactions;
drop table department_account;
drop table process_account;
drop table assembly_account;
drop table fit_job;
drop table paint_job;
drop table cut_job;
drop table job;
drop table fit_process;
drop table paint_process;
drop table cut_process;
drop table pass_through;
drop table supervise;
drop table process;
drop table orders;
drop table customer;
drop table department;
drop table assemblies;

-- MAKE TABLES


create table customer (
    cname varchar(40) primary key,
    caddress varchar(40) not null,
    category int not null

    constraint category_range_check
    check(category >= 1 and category <=10)
)


create table assemblies (
    aid int primary key,
    date_order DATE not null,
    detail varchar(40)
)

create table orders (
    cname varchar(40) not null,
    aid int primary key

    constraint FK_customer_orders 
    foreign key (cname) references customer(cname),

    constraint FK_assembly_orders
    foreign key (aid) references assemblies(aid)
)

create table process (
    pid int primary key,
    pdata varchar(5)

    constraint process_type_check
    check(pdata = 'Paint' OR pdata = 'Fit' OR pdata = 'Cut')
)

create table department(
    dnum int primary key,
    ddata varchar(40)
)

create table supervise (
    pid int primary key,
    dnum int not null

    constraint FK_process_supervise 
    foreign key (pid) references process(pid),

    constraint FK_department_supervise
    foreign key (dnum) references department(dnum)
)

create table pass_through (
    aid int not null,
    pid int not null
    primary key (aid, pid)

    constraint FK_assembly_pass_through
    foreign key (aid) references assemblies(aid),

    constraint FK_process_pass_through
    foreign key (pid) references process(pid)
)

create table cut_process (
    pid int primary key,
    machine_type varchar(40)

    constraint FK_process_cut_process
    foreign key (pid) references process(pid)
)

create table paint_process (
    pid int primary key,
    method varchar(40)

    constraint FK_process_paint_process
    foreign key (pid) references process(pid)
)

create table fit_process (
    pid int primary key,

    constraint FK_process_fit_process
    foreign key (pid) references process(pid)
)

create table job (
    jnum int primary key,
    date_commence date not null,
    date_complete date,
    aid int not null,
    pid int not null,
    info varchar(5)

    constraint FK_assemblies_job
    foreign key (aid) references assemblies(aid),

    constraint FK_process_job
    foreign key (pid) references process(pid),

    constraint job_type_check
    check(info = 'Paint' OR info = 'Fit' OR info = 'Cut')
)

create table cut_job (
    jnum int primary key,
    machine_type varchar(40) not null,
    machine_time int,
    material varchar(40),
    labor_time int

    constraint FK_job_cut_job
    foreign key (jnum) references job(jnum),

    constraint cut_job_time_range_check
    check(labor_time >= 0 and machine_time >= 0)
)

create table paint_job (
    jnum int primary key,
    color varchar(40),
    volume int,
    labor_time int

    constraint FK_job_paint_job
    foreign key (jnum) references job(jnum),

    constraint paint_job_time_range_check
    check(labor_time >= 0 and volume >= 0)
)

create table fit_job (
    jnum int primary key,
    labor_time int

    constraint FK_job_fit_job
    foreign key (jnum) references job(jnum),

    constraint fit_job_time_range_check
    check(labor_time >= 0)
)

create table assembly_account (
    aanum int primary key,
    date_established date not null,
    cost float,
    aid int not null

    constraint FK_assemblies_assembly_account
    foreign key (aid) references assemblies(aid),

    constraint assembly_account_cost_range_check
    check(cost >= 0)
)

create table process_account (
    panum int primary key,
    date_established date not null,
    cost float,
    pid int not null

    constraint FK_process_process_account
    foreign key (pid) references process(pid),

    constraint process_account_cost_range_check
    check(cost >= 0)
)

create table department_account (
    danum int primary key,
    date_established date not null,
    cost float,
    dnum int not null

    constraint FK_department_department_account
    foreign key (dnum) references department(dnum),

    constraint department_account_cost_range_check
    check(cost >= 0)
)

create table transactions (
    tnum int primary key,
    sup_cost float not null,

    constraint transactions_cost_check
    check(sup_cost >= 0)
)

create table record(
    tnum int primary key,
    jnum int not null

    constraint FK_transactions_record
    foreign key (tnum) references transactions(tnum),

    constraint FK_job_record
    foreign key (jnum) references job(jnum)
)

create table updates (
    tnum int primary key,
    aanum int not null,
    panum int not null,
    danum int not null

    constraint FK_transactions_updates
    foreign key (tnum) references transactions(tnum),

    constraint FK_assembly_account_updates
    foreign key (aanum) references assembly_account(aanum),

    constraint FK_process_account_updates
    foreign key (panum) references process_account(panum),

    constraint FK_department_account_updates
    foreign key (danum) references department_account(danum)

)

create table assembly_track (
    aid int primary key,
    aanum int not null

    constraint FK_assemblies_assembly_track
    foreign key (aid) references assemblies(aid),

    constraint FK_assembly_account_assembly_track
    foreign key (aanum) references assembly_account(aanum)
)

create table process_track (
    pid int primary key,
    panum int not null

    constraint FK_process_process_track
    foreign key (pid) references process(pid),

    constraint FK_process_account_process_track
    foreign key (panum) references process_account(panum)
)

create table department_track (
    dnum int primary key,
    danum int not null

    constraint FK_department_department_track
    foreign key (dnum) references department(dnum),

    constraint FK_department_account_department_track
    foreign key(danum) references department_account(danum)
)


--File organization, for indexing


create NONCLUSTERED INDEX cname
on customer (cname)
create NONCLUSTERED INDEX category
on customer (category)


create NONCLUSTERED INDEX aid
on assemblies (aid)

create NONCLUSTERED INDEX pid
on process (pid)

create NONCLUSTERED INDEX dnum
on department(dnum)

create NONCLUSTERED INDEX pid
on supervise(pid)
create NONCLUSTERED INDEX dnum
on supervise(dnum)

create NONCLUSTERED INDEX machine_type
on cut_process (machine_type)

create NONCLUSTERED INDEX jnum
on job (jnum)

create NONCLUSTERED INDEX jnum
on cut_job(jnum)

create NONCLUSTERED INDEX jnum
on paint_job(jnum)

create NONCLUSTERED INDEX jnum
on fit_job(jnum)

create NONCLUSTERED INDEX aanum
on assembly_account(aanum)

create NONCLUSTERED INDEX panum
on process_account(panum)

create NONCLUSTERED INDEX danum
on department_account(danum)