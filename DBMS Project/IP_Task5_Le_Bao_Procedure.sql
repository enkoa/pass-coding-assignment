--Procedure for JAVA


drop procedure insert_customer;
drop procedure insert_department;
drop procedure insert_assembly;
drop procedure insert_process;
drop procedure insert_accounts;
drop procedure insert_assembly_account;
drop procedure insert_process_account;
drop procedure insert_department_account;
drop procedure insert_job;
drop procedure insert_job_done;
drop procedure update_job_info
drop procedure insert_trans;
drop procedure assembly_cost;
drop procedure labor_time;
drop procedure assembly_pass;
drop procedure job_complete;
drop procedure get_customer;
drop procedure delete_cut;
drop procedure change_color;

-- query 1

GO
CREATE PROCEDURE insert_customer
    @cname VARCHAR(40),
    @caddress VARCHAR(40),
    @category INT
AS
BEGIN
    INSERT INTO customer(
        cname, caddress, category
    ) values
        (@cname, @caddress, @category)
END

--query 2

GO
CREATE PROCEDURE insert_department
    @dnum INT,
    @ddata VARCHAR(40)
AS
BEGIN
    INSERT INTO department(
        dnum, ddata
    ) values
        (@dnum, @ddata)
END

--query 3

GO
CREATE PROCEDURE insert_assembly
    @aid INT,
    @date_order VARCHAR(40),
    @detail VARCHAR(40),
    @cname VARCHAR(40)
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM customer WHERE cname = @cname)
        THROW 50000, 'Customer does not exist',1
    ELSE 
        BEGIN TRY
        INSERT INTO assemblies(
            aid, date_order, detail
        ) values
            (@aid, @date_order, @detail)
        INSERT INTO orders(
         aid, cname
        ) values
            (@aid, @cname)
        END TRY
        BEGIN CATCH
            THROW 50000, 'Assembly ID already exist', 1
        END CATCH
END

--query 4

GO
CREATE PROCEDURE insert_process
    @pid INT,
    @pdata VARCHAR(40),
    @dnum INT,
    @info VARCHAR(40)
AS
BEGIN
    -- check if department exists
    IF NOT EXISTS (SELECT 1 FROM department WHERE dnum = @dnum)
        THROW 50001, 'Department number not found', 1
    ELSE
        BEGIN TRY
            IF @pdata = '1'
            BEGIN
                INSERT INTO process (pid, pdata) values (@pid, 'Cut')
                INSERT INTO supervise (pid, dnum) values (@pid, @dnum)
                INSERT INTO cut_process (pid, machine_type) values (@pid, @info)
            END
            IF @pdata = '2'
            BEGIN
                INSERT INTO process (pid, pdata) values (@pid, 'Paint')
                INSERT INTO supervise (pid, dnum) values (@pid, @dnum)
                INSERT INTO paint_process (pid, method) values (@pid, @info)
            END
            IF @pdata = '3'
            BEGIN
                INSERT INTO process (pid, pdata) values (@pid, 'Fit')
                INSERT INTO supervise (pid, dnum) values (@pid, @dnum)
                INSERT INTO fit_process (pid) values (@pid)
            END
        END TRY
        BEGIN CATCH
            THROW 50001, 'Process ID already exists', 1
        END CATCH
END

-- query 5

GO
CREATE PROCEDURE insert_accounts
    @account_id INT,
    @assoc INT,
    @pk INT,
    @date VARCHAR(40)
AS
BEGIN
    -- make sure other accounts aren't using this id
    IF EXISTS (SELECT 1 FROM assembly_track WHERE aanum = @account_id)
        THROW 50003, 'This Account number has already been used for an Assembly Account',1
    ELSE IF EXISTS (SELECT 1 FROM process_track WHERE panum = @account_id)
        THROW 50004, 'This Account number has already been used for a Process Account',1
    ELSE IF EXISTS (SELECT 1 FROM department_track WHERE danum = @account_id)
        THROW 50005, 'This Account number has already been used for a Department Account',1
    ELSE
        IF @assoc = 1
            EXEC insert_assembly_account @account_id, @pk, @date
        ELSE IF @assoc = 2
            EXEC insert_process_account @account_id, @pk, @date
        ELSE 
            EXEC insert_department_account @account_id, @pk, @date
END
-- support assembly
GO
CREATE PROCEDURE insert_assembly_account
    @account_id INT,
    @pk INT,
    @date VARCHAR(40)
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM assemblies WHERE aid = @pk)
        THROW 50003, 'Assembly ID does not exist',1
    ELSE IF EXISTS (SELECT 1 FROM assembly_track WHERE aid = @pk)
        THROW 50003, 'Assembly ID already have an associated account',1
    ELSE
        BEGIN
            INSERT INTO assembly_account (aanum, date_established, cost, aid) VALUES (@account_id, @date, '0.00', @pk)
            INSERT INTO assembly_track (aanum, aid) VALUES (@account_id, @pk)
        END
END
--suport process
GO
CREATE PROCEDURE insert_process_account
    @account_id INT,
    @pk INT,
    @date VARCHAR(40)
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM process WHERE pid = @pk)
        THROW 50004, 'Process ID does not exist',1
    ELSE IF EXISTS (SELECT 1 FROM process_track WHERE pid = @pk)
        THROW 50004, 'Process ID already have an associated account',1
    ELSE
        BEGIN
            INSERT INTO process_account (panum, date_established, cost, pid) VALUES (@account_id, @date, '0.00', @pk)
            INSERT INTO process_track (panum, pid) VALUES (@account_id, @pk)
        END
END
--suport department
GO
CREATE PROCEDURE insert_department_account
    @account_id INT,
    @pk INT,
    @date VARCHAR(40)
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM department WHERE dnum = @pk)
        THROW 50005, 'Department number does not exist',1
    ELSE IF EXISTS (SELECT 1 FROM department_track WHERE dnum = @pk) 
        THROW 50005, 'Department number already have an associated account',1
    ELSE
        BEGIN
            INSERT INTO department_account (danum, date_established, cost, dnum) VALUES (@account_id, @date, '0.00', @pk)
            INSERT INTO department_track (danum, dnum) VALUES (@account_id, @pk)
        END
END

-- query 6

GO 
CREATE PROCEDURE insert_job
    @jnum INT,
    @aid INT,
    @pid INT,
    @date DATE
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM assemblies WHERE aid = @aid)
        THROW 50006, 'Assembly ID does not exists',1
    ELSE IF NOT EXISTS (SELECT 1 FROM process WHERE pid = @pid)
        THROW 50006, 'Process ID does not exists',1
    ELSE
        BEGIN TRY
            INSERT INTO job (jnum, date_commence, aid, pid) VALUES (@jnum, @date, @aid, @pid)
        END TRY
        BEGIN CATCH
                THROW 50006, 'Job number already exists',1
        END CATCH
        BEGIN TRY
            INSERT INTO pass_through(pid, aid) VALUES (@pid, @aid)
        END TRY
        BEGIN CATCH
                THROW 50006, 'Assembly passing through this process once more',1
        END CATCH
END


-- query 7

GO
CREATE PROCEDURE insert_job_done
    @jnum INT,
    @date DATE
AS
BEGIN
    IF EXISTS (SELECT date_complete FROM job WHERE jnum = @jnum AND date_complete IS NOT NULL) 
        THROW 50007, 'This job has already been completed',1
    ELSE
    BEGIN TRY 
        UPDATE job SET date_complete = @date WHERE jnum = @jnum
        SELECT process.pdata FROM job, process WHERE @jnum = job.jnum AND job.pid = process.pid
    END TRY
    BEGIN CATCH
        THROW 50007, 'Job number does not exists',1
    END CATCH
END
-- update remaining job information after clarification on type of process
GO 
CREATE PROCEDURE update_job_info
    @jnum INT,
    @type VARCHAR(40),
    @labor INT,
    @info1 VARCHAR(40),
    @info2 VARCHAR(40)
AS
BEGIN
    BEGIN TRY
    UPDATE job SET info = @type WHERE jnum = @jnum
    IF @type = 'Cut'
        INSERT INTO cut_job (jnum, machine_type, machine_time, material, labor_time) 
        VALUES (@jnum, (SELECT machine_type FROM job, cut_process WHERE @jnum = job.jnum AND job.pid = cut_process.pid), @info1, @info2, @labor)
    ELSE IF @type = 'Paint'
        INSERT INTO paint_job (jnum, color,volume, labor_time) VALUES (@jnum, @info1, @info2, @labor)
    ELSE IF @type = 'Fit'
        INSERT INTO fit_job (jnum, labor_time) VALUES (@jnum, @labor)
    END TRY
    BEGIN CATCH
        THROW 50008, 'Odd, how did we end up here?',1
    END CATCH
END

-- query 8

GO
CREATE PROCEDURE insert_trans
    @tnum INT,
    @jnum INT,
    @cost FLOAT
AS
BEGIN
    DECLARE @aanum INT = (SELECT assembly_account.aanum FROM job, assembly_account WHERE @jnum = job.jnum AND job.aid = assembly_account.aid)
    DECLARE @panum INT = (SELECT process_account.panum FROM job, process_account WHERE @jnum = job.jnum AND job.pid = process_account.pid)
    DECLARE @danum INT = (SELECT department_account.danum FROM job, department_account, supervise WHERE @jnum = job.jnum AND job.pid = supervise.pid AND supervise.dnum = department_account.dnum)
    
    IF NOT EXISTS (SELECT 1 FROM job WHERE @jnum = jnum)
        THROW 50009,'Job number does not exists',1
    ELSE IF @aanum IS NULL
        THROW 50009, 'Assembly for this job does not have a registered account',1
    ELSE IF @panum IS NULL
        THROW 50009, 'Process for this job does not have a registered account',1
    ELSE IF @danum IS NULL
        THROW 50009, 'Department for the process of this job does not have a registered account',1
    ELSE 
        BEGIN TRY
            INSERT INTO transactions (tnum, sup_cost) VALUES (@tnum, @cost)
            INSERT INTO record (tnum, jnum) VALUES (@tnum, @jnum)
            INSERT INTO updates (tnum, aanum, panum, danum) VALUES (@tnum, @aanum, @panum, @danum)
            UPDATE assembly_account SET cost = cost + @cost WHERE aanum = @aanum
            UPDATE process_account SET cost = cost + @cost WHERE panum = @panum
            UPDATE department_account SET cost = cost + @cost WHERE danum = @danum
        END TRY
        BEGIN CATCH
            THROW 50009, 'Transaction number already existed',1
        END CATCH
END

-- query 9

GO
CREATE PROCEDURE assembly_cost
    @aid INT
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM assemblies WHERE aid = @aid) 
        THROW 50010, 'Assembly ID does not exist',1
    ELSE IF NOT EXISTS (SELECT 1 FROM assembly_account WHERE aid = @aid)
        THROW 50010, 'Assembly ID does not have an account',1
    ELSE
        SELECT cost FROM assembly_account WHERE aid = @aid
END

-- query 10

GO 
CREATE PROCEDURE labor_time
    @dnum INT,
    @date DATE
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM department WHERE dnum = @dnum)
        THROW 50011, 'Department number does not exist',1
    ELSE
        DECLARE @fit_sum INT = 
        (SELECT SUM(fit_job.labor_time)
        FROM supervise, job,fit_job
        WHERE supervise.dnum = @dnum AND supervise.pid = job.pid AND job.date_complete = @date AND
            fit_job.jnum = job.jnum);
        
        DECLARE @paint_sum INT =
        (SELECT DISTINCT SUM(cut_job.labor_time)
        FROM supervise, job,cut_job
        WHERE supervise.dnum = @dnum AND supervise.pid = job.pid AND job.date_complete = @date AND
            cut_job.jnum = job.jnum);

        DECLARE @cut_sum INT =
        (SELECT DISTINCT SUM(paint_job.labor_time)
        FROM supervise, job,paint_job
        WHERE supervise.dnum = @dnum AND supervise.pid = job.pid AND job.date_complete = @date AND
            paint_job.jnum = job.jnum);

        DECLARE @labor_sum INT = 0;
        IF @fit_sum IS NOT NULL
            SET @labor_sum = @labor_sum + @fit_sum;
        IF @paint_sum IS NOT NULL
            SET @labor_sum = @labor_sum + @paint_sum;
        IF @cut_sum IS NOT NULL
            SET @labor_sum = @labor_sum + @cut_sum;

        SELECT @labor_sum AS RESULT
END

-- query 11

GO
CREATE PROCEDURE assembly_pass
    @aid INT
AS 
BEGIN
/*
    IF NOT EXISTS (SELECT 1 FROM assemblies WHERE aid = @aid)
        THROW 50012,'Assembly ID does not exists',1
    ELSE
*/
    SELECT job.date_commence, job.pid, supervise.dnum
    FROM job, supervise
    WHERE job.aid = @aid AND job.pid = supervise.pid
    ORDER BY job.date_commence ASC
END

-- query 12

GO
CREATE PROCEDURE job_complete
    @dnum INT,
    @date DATE
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM department WHERE dnum = @dnum)
        THROW 50013,'Department number does not exists',1
    ELSE
        SELECT ALL job.jnum, job.info, job.aid
        FROM job, supervise
        WHERE supervise.dnum = @dnum AND supervise.pid = job.pid AND job.date_complete = @date
END

--query 13

GO
CREATE PROCEDURE get_customer
    @lb INT,
    @ub INT
AS
BEGIN
    IF @lb > @ub OR @lb < 1 OR @ub > 10
        THROW 50014,'Requested bound is not correct',1
    ELSE
        SELECT customer.cname, customer.caddress, customer.category
        FROM customer
        WHERE customer.category >= @lb AND customer.category <= @ub
        ORDER BY customer.cname ASC
END

--query 14

GO
CREATE PROCEDURE delete_cut
    @joblb INT,
    @jobub INT
AS
BEGIN
    IF @joblb > @jobub
        THROW 50015,'Job number lower bound cannot be larger than job number upper bound',1
    ELSE
        ALTER TABLE record nocheck constraint all

        DELETE FROM cut_job
        WHERE cut_job.jnum >= @joblb AND cut_job.jnum <= @jobub
        DELETE FROM job
        WHERE job.jnum >= @joblb AND job.jnum <= @jobub AND job.info = 'Cut'

        ALTER TABLE record check constraint all
END

-- query 15

GO
CREATE PROCEDURE change_color
    @jnum INT,
    @color VARCHAR(40)
AS
BEGIN
    IF NOT EXISTS (SELECT 1 FROM job WHERE jnum = @jnum AND info = 'Paint')
        THROW 50016,'Paint job with this job number does not exist',1
    ELSE 
        UPDATE paint_job SET color = @color WHERE jnum = @jnum
END
