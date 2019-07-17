using Microsoft.EntityFrameworkCore;
using System.Collections.Generic;

namespace CoreBrewServer.Models 
{
    public class BrewingContext : DbContext
    {
        public BrewingContext(DbContextOptions<BrewingContext> options)
            :base(options)
            { }

        public DbSet<BrewDay> BrewDays {get; set;}
        public DbSet<BrewSchedule> BrewSchedules {get; set;}
        public DbSet<MashTemperature> MashTemperatures {get; set;}
    }
}